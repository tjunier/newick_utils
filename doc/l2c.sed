# sed commands to help converting files from LaTeX to ConTeXt - note that this
# will just do some of the job, it will by no means perform a full conversion.

# NOTES:

# Don't remove labels automatically. There is a map (,xl) in Session.vim for
# adding # labels to th epreceding command, it may work.

# \url{} must be changed to\url[], but this takes a name that refers to a URL,
# which must be defined using \useURL, somewhere before \starttext.

# Sectioning 
s/\\chapter\*/\\chapter/
s/\\section\*/\\section/
s/\\subsection\*/\\subsection/

# Environments
s/\\begin{itemize}/\\startitemize/
s/\\end{itemize}/\\stopitemize/
s/\\begin{verbatim}/\\starttyping/
s/\\end{verbatim}/\\stoptyping/
s/\\begin{samepage}/\\page[no]/
s/\\end{samepage}//

# Other commands
s/\\verbatiminput{/\\typefile{/

# Formatting, fonts, etc.
s/\\emph{/{\\em /g
s/\\textsc/\\cap/g

# Cross-references
s/section \\ref{\([A-Za-z0-9_-]\+\)}/\\in{section}[\1]/g
s/figure \\ref{\([A-Za-z0-9_-]\+\)}/\\in{figure}[\1]/g
s/table \\ref{\([A-Za-z0-9_-]\+\)}/\\in{table}[\1]/g
s/\\ref{\([A-Za-z0-9_-]\+\)}/\\in{}[\1]/g
