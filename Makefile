all: main

main:
	@printf -- "--- ERRMSG ---\n"
	$(MAKE) -C errmsg $@
	@printf -- "--------------\n\n"
	@printf -- "--- LINKEDLIST ---\n"
	$(MAKE) -C linkedlist $@
	@printf -- "------------------\n\n"
	@printf -- "--- HASHMAP ---\n"
	$(MAKE) -C hashmap $@
	@printf -- "---------------\n\n"
	@printf -- "--- STACK ---\n"
	$(MAKE) -C stack $@
	@printf -- "-------------\n\n"
	@printf -- "--- QUEUE ---\n"
	$(MAKE) -C queue $@
	@printf -- "-------------\n\n"
	@printf -- "--- PRIORITY QUEUE ---\n"
	$(MAKE) -C priorityqueue $@
	@printf -- "----------------------\n\n"
	@printf -- "--- GAME ---\n"
	$(MAKE) -C game $@
	@printf -- "------------\n\n"

clean:
	@printf -- "--- ERRMSG ---\n"
	$(MAKE) -C errmsg $@
	@printf -- "--------------\n\n"
	@printf -- "--- LINKEDLIST ---\n"
	$(MAKE) -C linkedlist $@
	@printf -- "------------------\n\n"
	@printf -- "--- HASHMAP ---\n"
	$(MAKE) -C hashmap $@
	@printf -- "---------------\n\n"
	@printf -- "--- STACK ---\n"
	$(MAKE) -C stack $@
	@printf -- "-------------\n\n"
	@printf -- "--- QUEUE ---\n"
	$(MAKE) -C queue $@
	@printf -- "-------------\n\n"
	@printf -- "--- PRIORITY QUEUE ---\n"
	$(MAKE) -C priorityqueue $@
	@printf -- "----------------------\n\n"
	@printf -- "--- GAME ---\n"
	$(MAKE) -C game $@
	@printf -- "------------\n\n"

install:
	@printf -- "--- ERRMSG ---\n"
	$(MAKE) -C errmsg $@
	@printf -- "--------------\n\n"
	@printf -- "--- LINKEDLIST ---\n"
	$(MAKE) -C linkedlist $@
	@printf -- "------------------\n\n"
	@printf -- "--- HASHMAP ---\n"
	$(MAKE) -C hashmap $@
	@printf -- "---------------\n\n"
	@printf -- "--- STACK ---\n"
	$(MAKE) -C stack $@
	@printf -- "-------------\n\n"
	@printf -- "--- QUEUE ---\n"
	$(MAKE) -C queue $@
	@printf -- "-------------\n\n"
	@printf -- "--- PRIORITY QUEUE ---\n"
	$(MAKE) -C priorityqueue $@
	@printf -- "----------------------\n\n"
	@printf -- "--- GAME ---\n"
	$(MAKE) -C game $@
	@printf -- "------------\n\n"
