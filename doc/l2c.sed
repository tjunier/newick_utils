# sed commands to help converting files from LaTeX to ConTeXt - note that this
# will just do some of the job, it will by no means perform a full conversion.

# NOTES:

# Don't remove labels automatically. There is a map (,xl) in l2c.vim for
# adding labels to the preceding command, it may work.

# \url{} must be changed to\url[], but this takes a name that refers to a URL,
# which must be defined using \useURL, somewhere before \starttext.

# We don't convert \textit to \it, because it's better to use semantic
# formatting (see nuenv.tex).

# Sectioning 
s/\\chapter\*/\\chapter/
s/\\section\*/\\section/
s/\\subsection\*/\\subsection/

# Environments
s/\\begin{itemize}/\\startitemize/
s/\\end{itemize}/\\stopitemize/
s/\\begin{enumerate}/\\startitemize[n]/
s/\\end{enumerate}/\\stopitemize/
s/\\begin{verbatim}/\\starttyping/
s/\\end{verbatim}/\\stoptyping/
s/\\begin{samepage}/\\page[no]/
s/\\end{samepage}//
s/\\begin{center}/\\startalignment[center]/
s/\\end{center}/\\stopalignment/
s/\\begin{quote}/\\startnarrower/
s/\\end{quote}/\\stopnarrower/

# Fixed expressions
s/\\textit{e.g.}/\\eg{}/g

# Other commands
s/\\verbatiminput{/\\typefile{/
s/\\includegraphics{\([A-Za-z0-9_-]\+\)\....}/\\externalfigure[\1]/g
s/\\includegraphics\[\([A-Za-z0-9_=.-]\+\)]{\([A-Za-z0-9_-]\+\)\....}/\\externalfigure[\2][\1]/g
s/\\verb+\(.*\)+/\\type{\1}/g
s/\\noindent{}//g

# Formatting, fonts, etc.
s/\\emph{/{\\em /g
s/\\textsc/\\cap/g
s/\\footnotesize/\\tfx/g

# Cross-references
s/section \\ref{\([A-Za-z0-9_:-]\+\)}/\\in{section}[\1]/g
s/figure \\ref{\([A-Za-z0-9_:-]\+\)}/\\in{figure}[\1]/g
s/table \\ref{\([A-Za-z0-9_:-]\+\)}/\\in{table}[\1]/g
s/\\ref{\([A-Za-z0-9_:-]\+\)}/\\in{}[\1]/g
