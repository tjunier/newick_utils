" takes the label from a \label command, and puts it in the preceding line
" (which is hopefully a sectioning command, or something) as the label.
"
" Run l2c.sed first! Call when cursor is on '\' or in "label" word. 

:map ,xl f{wd/}<CR>ddk0f{i[<ESC>"2pa]<ESC>
