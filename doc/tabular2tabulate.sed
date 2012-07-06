# Sed commands for converting LaTeX tabular to ConTeXt tabulate. Don't run this
# on a whole file, run it just on the tabular environment code. You still have
# to take care of (e.g.) captions, labels, etc.; and add the first column of
# \NRs.

s/\\begin{table}.*/\\placetable[here][]{}/
s/\\begin{tabular}{\(.*\)}/\\starttabulate[\1]/
s/\\hline/\\HL/
s/&/\\NC/g
s/\\\\/\\NC\\NR/
s/\\end{tabular}/\\stoptabulate/
s/\\end{table}//
