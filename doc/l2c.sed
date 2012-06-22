# sed commands to help converting files from LaTeX to ConTeXt - note that this
# will just do some of the job, it will by no means perform a full conversion.

# Sectioning 
s/\\chapter\*/\\chapter/

# Environments
s/\\begin{itemize}/\\startitemize/
s/\\end{itemize}/\\stopitemize/
s/\\begin{verbatim}/\\starttyping/
s/\\end{verbatim}/\\stoptyping/

# Formatting, fonts, etc.
s/\\emph/\\em/g

# Cross-references
s/section \\ref{\([A-Za-z0-9_-]\+\)}/\\in{section}[\1]/g
s/figure \\ref{\([A-Za-z0-9_-]\+\)}/\\in{figure}[\1]/g
s/table \\ref{\([A-Za-z0-9_-]\+\)}/\\in{table}[\1]/g
s/\\ref{\([A-Za-z0-9_-]\+\)}/\\in{}[\1]/g
