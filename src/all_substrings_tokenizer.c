#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include <mb/pg_wchar.h>

PG_MODULE_MAGIC;

#define MIN_CHARS_IN_WORD 3

PG_FUNCTION_INFO_V1(all_substrings_set);

typedef struct {
  int start;
  int end;
} SubstrContext;

Datum
all_substrings_set(PG_FUNCTION_ARGS)
{
  FuncCallContext *funcctx;
  TupleDesc        tupdesc;
  AttInMetadata   *attinmeta;
  SubstrContext   *substrctx;
  text            *str = PG_GETARG_TEXT_P(0);

  int input_size = VARSIZE(str) - VARHDRSZ;

  /* stuff done only on the first call of the function */
  if (SRF_IS_FIRSTCALL())
  {
    MemoryContext   oldcontext;

    /* create a function context for cross-call persistence */
    funcctx = SRF_FIRSTCALL_INIT();

    /* switch to memory context appropriate for multiple function calls */
    oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

    /* Build a tuple descriptor for our result type */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                  errmsg("function returning record called in context "
                        "that cannot accept type record")));

    /*
      * generate attribute metadata needed later to produce tuples from raw
      * C strings
      */
    attinmeta = TupleDescGetAttInMetadata(tupdesc);
    funcctx->attinmeta = attinmeta;

    substrctx = (SubstrContext *) palloc(sizeof(SubstrContext));
    substrctx->start = 0;
    substrctx->end = 0;

    int i;
    for (i = 0; i < MIN_CHARS_IN_WORD; i++) {
      /* return if the text is too small to be indexed. */
      if (input_size == substrctx->end) {
        SRF_RETURN_DONE(funcctx);
      }

      substrctx->end += pg_mblen(VARDATA(str) + substrctx->end);
    }

    funcctx->user_fctx = substrctx;
    MemoryContextSwitchTo(oldcontext);
  }

  /* stuff done on every call of the function */
  funcctx = SRF_PERCALL_SETUP();
  attinmeta = funcctx->attinmeta;

  substrctx = funcctx->user_fctx;

  int t = substrctx->start;
  int i;
  for (i = 0; i < MIN_CHARS_IN_WORD; i++) {
    t += pg_mblen(VARDATA(str) + t);
  }

  if (t > input_size) {
    SRF_RETURN_DONE(funcctx);
  }

  char       **values;
  HeapTuple    tuple;
  Datum        result;

  int substr_byte_length = substrctx->end - substrctx->start;

  values = (char **) palloc0(1 * sizeof(char *));
  values[0] = (char *) palloc0((substr_byte_length + 1) * sizeof(char));
  memcpy(values[0], VARDATA(str) + substrctx->start, (substr_byte_length) * sizeof(char));

  if (substrctx->end == input_size) {
    substrctx->start += pg_mblen(VARDATA(str) + substrctx->start);
    substrctx->end = substrctx->start;
    for (i = 0; i < MIN_CHARS_IN_WORD; i++) {
      substrctx->end += pg_mblen(VARDATA(str) + substrctx->end);
    }
  } else {
    substrctx->end += pg_mblen(VARDATA(str) + substrctx->end);
  }

  /* build a tuple */
  tuple = BuildTupleFromCStrings(attinmeta, values);

  /* make the tuple into a datum */
  result = HeapTupleGetDatum(tuple);

  /* clean up (this is not really necessary) */
  pfree(values[0]);
  pfree(values);

  SRF_RETURN_NEXT(funcctx, result);
}
