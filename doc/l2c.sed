# sed commands to help converting files from LaTeX to ConTeXt - note that this
# will just do some of the job, it will by no means perform a full conversion.
s/\\chapter\*/\\chapter/

s/\\begin{itemize}/\\startitemize/
s/\\end{itemize}/\\stopitemize/
s/\\emph/\\em/g
