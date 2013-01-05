" takes the label from a \label command, and puts it in the preceding line
" (which is hopefully a sectioning command, or something) as the label.
"
" Run l2c.sed first! Call when cursor is in the line that has the \label cmd.

map ,xl $x0df{d$k0f{i[<Esc>pa]<Esc>

map ,to :g/typefile.*txt\.cmd/s/typefile/txtCmdOutput/<CR>:g/txtCmdOutput.*cmd/s/_txt.cmd//<CR>
map ,so :g/typefile.*svg\.cmd/s/typefile/svgCmdOutput/<CR>:g/svgCmdOutput.*cmd/s/_svg.cmd//<CR>
