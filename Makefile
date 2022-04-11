GIT_HOOK := .git/hooks/applied
$(GIT_HOOK): scripts/install-git-hooks
	@$<
	@echo

.PHONY: all check examples clean
all: $(GIT_HOOK) check
.DEFAULT_GOAL := all

include common.mk

CFLAGS = -I./include -I./private
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Werror

TESTS = \
    containerof \
    list_entry \
    list_init-explicit \
    list_init-local \
    list_init-global \
    list_add \
    list_add_tail \
    list_del \
    list_del_init \
    list_first_entry \
    list_last_entry \
    list_is_singular \
    list_for_each_safe \
    list_for_each_entry_safe \
    list_for_each \
    list_for_each_entry \
    list_move \
    list_move_tail \
    list_splice \
    list_splice_tail \
    list_splice_init \
    list_splice_tail_init \
    list_cut_position

EXAMPLES = \
	insert-sort \
	quick-sort
	#quick-sort-non-recur \

TESTS := $(addprefix tests/,$(TESTS))
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# dependency of source files
deps := $(TESTS:%=%.o.d)

# Different ways but results are the same
#TESTS_OK = $(addsuffix .ok,${TESTS})
#TESTS_OK = $(TESTS:%=%.ok)

# By implict rule?
TESTS_OK = $(TESTS:=.ok)

check: $(TESTS_OK)
examples: $(EXAMPLES)

$(TESTS_OK): %.ok: %
	$(Q)$(PRINTF) "*** Validating $< ***\n"
	$(Q)./$< && $(PRINTF) "\t$(PASS_COLOR)[ Verified ]$(NO_COLOR)\n"
	@touch $@

# standard build rules
.SUFFIXES: .o .c
# two-suffix rule == '%.o : %.c'
.c.o:
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF $@.d $<

# Static Pattern Rules
$(TESTS): %: %.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

$(EXAMPLES): %: %.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(VECHO) "  Cleaning...\n"
	$(Q)$(RM) $(TESTS) $(EXAMPLES) $(TESTS_OK) $(TESTS:=.o) \
	          $(EXAMPLES:=.0) $(deps)

-include $(deps)
