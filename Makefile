SUBDIRS = src

.PHONY: clean subdirs $(SUBDIRS)

seq_poly:
	$(MAKE) -s -C $(SUBDIRS) seq_poly

seq_img:
	$(MAKE) -s -C $(SUBDIRS) seq_img

par_poly:
	$(MAKE) -s -C $(SUBDIRS) par_poly

clean:
	$(MAKE) -C $(SUBDIRS) clean

run_seq:
	$(MAKE) -s -C $(SUBDIRS) run_seq

run_par:
	$(MAKE) -s -C $(SUBDIRS) run_par
