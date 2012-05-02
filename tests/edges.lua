
-- prints edges as (lbl, parent.lbl)
function node()
	if not r then
		e = string.format('("%s" "%s")' , lbl, N.par.lbl)
		print(e)
	end
end
