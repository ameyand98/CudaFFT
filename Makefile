SUBDIRS = src

.PHONY: clean subdirs $(SUBDIRS)

seq:
	$(MAKE) -C $(SUBDIRS) seq

par:
	$(MAKE) -C $(SUBDIRS) par

clean:
	$(MAKE) -C $(SUBDIRS) clean

run_seq:
	$(MAKE) -C $(SUBDIRS) run_seq

run_par:
	$(MAKE) -C $(SUBDIRS) run_par
