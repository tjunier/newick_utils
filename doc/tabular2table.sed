# Sed commands for converting LaTeX tabular to TaBlE used by ConTeXt. Don't run
# this on a whole file, run it just on the tabular environment code. You still
# have to take care of (e.g.) captions, labels, etc.; and add the first column
# of \NRs.

s/\\begin{tabular}/\\starttable[here][]{}/
s/\\hline/\\HL/
s/&/\\NC/g
s/\\\\/\\MR/
s/\\end{tabular}/\\stoptable/
