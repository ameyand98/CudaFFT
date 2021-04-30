SUBDIRS = src

.PHONY: clean subdirs $(SUBDIRS)

default:
	$(MAKE) -C $(SUBDIRS)

clean:
	$(MAKE) -C $(SUBDIRS) clean

run:
	$(MAKE) -C $(SUBDIRS) run