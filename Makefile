SUBDIRS = src

.PHONY: clean subdirs $(SUBDIRS)

seq:
	$(MAKE) -s -C $(SUBDIRS) seq

par:
	@$(MAKE) -s -C $(SUBDIRS) par

clean:
	$(MAKE) -C $(SUBDIRS) clean

run_seq:
	$(MAKE) -s -C $(SUBDIRS) run_seq

run_par:
	@$(MAKE) -s -C $(SUBDIRS) run_par
