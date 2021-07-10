.SUFFIXES : .tex .pdf

PDFLATEX = pdflatex
DOCS_PDF = ManuX.pdf boot-part.pdf inter-part.pdf

all : $(DOCS_PDF)

livre : ManuX.pdf

.tex.pdf :
	$(PDFLATEX) $<

clean :
	rm -f *.aux *.log *.toc *.idx $(DOCS_PDF)

