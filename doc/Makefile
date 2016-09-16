ROFF	= groff -man -Thtml -U

all:
	@echo "Try the following target: man_pages"

man_pages:
	ln -s . man3
	cp ../*.3 .
	for f in *.3; echo $$f; do $(ROFF) $$f > $$f.html; done
	rm *.3 man3
	@echo "Now you need to import the HTML files into index.html"
