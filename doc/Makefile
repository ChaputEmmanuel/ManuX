.SUFFIXES : .tex .pdf .pan
#-------------------------------------------------------------------------------
# En cours de construction. Voir le LISEZ-MOI général
#-------------------------------------------------------------------------------

PANDOC_CSS = pandoc.css
DOXY_CFG = config.doxy
PDFLATEX = pdflatex
DOXYGEN = doxygen
PANDOC = pandoc
DOCS_PDF = ManuX.pdf boot-part.pdf inter-part.pdf memoire-part.pdf affichage-part.pdf
DOCS_PAN = $(DOCS_PDF:.pdf=.pan)

all : $(DOCS_PDF)

dox :
	$(DOXYGEN) $(DOXY_CFG)

pan : $(DOCS_PAN)

livre : ManuX.pdf

.tex.pdf :
	$(PDFLATEX) $<

# Pour générer le html en fonction des sources LaTeX 
.tex.pan :
	$(PANDOC) --metadata pagetitle=\"$(basename $<)\" --css=$(PANDOC_CSS) $< -f latex -t html  -s -o html/$(basename $<).html
	cp $(PANDOC_CSS) html

clean :
	rm -f *.aux *.log *.toc *.idx $(DOCS_PDF)
	rm -rf html/* latex/*

