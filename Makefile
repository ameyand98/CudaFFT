SUBDIRS = src

.PHONY: clean subdirs $(SUBDIRS)

seq_poly:
	$(MAKE) -s -C $(SUBDIRS) seq_poly

seq_img:
	$(MAKE) -s -C $(SUBDIRS) seq_img

par_poly:
	$(MAKE) -s -C $(SUBDIRS) par_poly

par_img:
	$(MAKE) -s -C $(SUBDIRS) par_img

clean:
	$(MAKE) -C $(SUBDIRS) clean

run_seq_poly:
	$(MAKE) -s -C $(SUBDIRS) run_seq_poly

run_par_poly:
	$(MAKE) -s -C $(SUBDIRS) run_par_poly

run_par_img:
	$(MAKE) -s -C $(SUBDIRS) run_par_img

