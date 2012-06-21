# sed commands to help converting files from LaTeX to ConTeXt - note that this
# will just do some of the job, it will by no means perform a full conversion.
s/\\chapter\*/\\chapter/

s/\\begin{itemize}/\\startitemize/
s/\\end{itemize}/\\stopitemize/
s/\\emph/\\em/g
s/section \\ref{\([A-Za-z0-9_-]\+\)}/\\in{section}[\1]/g
s/figure \\ref{\([A-Za-z0-9_-]\+\)}/\\in{figure}[\1]/g
s/table \\ref{\([A-Za-z0-9_-]\+\)}/\\in{table}[\1]/g
