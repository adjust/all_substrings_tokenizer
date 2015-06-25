EXTENSION = all_substrings_tokenizer
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
			   sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

DATA = $(wildcard sql/*--*.sql)
TESTS = setup $(filter-out test/sql/setup.sql, $(wildcard test/sql/*.sql))
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test/
OBJS = src/all_substrings_tokenizer.o

MODULE_big = all_substrings_tokenizer

all: concat

concat:
	echo > sql/$(EXTENSION)--$(EXTVERSION).sql
	cat $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql)) >> sql/$(EXTENSION)--$(EXTVERSION).sql

EXTRA_CLEAN = sql/$(EXTENSION)--$(EXTVERSION).sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
