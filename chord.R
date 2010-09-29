# I want to determine the number of decimal points I should reasonably use for
# angles (in degrees). This is because using "%g" gives me a slightly different
# result (3rd decimal place off by 1) on Ubuntu and Fedora, and this causes the
# tests to fail. If I can round to 2 decimal places without loss of resolution,
# I'm out of trouble. For this I need to know the effects of rounding. 

# The question I ask is therefore, for a given angle theta, how long can the
# radius r be before the chord reaches 1 pixel? (see
# http://en.wikipedia.org/wiki/Chord_%28geometry%29). If r is acceptably long,
# I can round off angles up to theta.

# Therefore, I will look at various values of theta and look at the chord
# function assuming 1 pixel. The length of the chord is given by:
#
# r crd(theta)
#
# where crd(theta) = 2 sin(theta/2)
#
# so, if we fix r crd(theta) == 1 (pixel), we find
#
# r = 1 / (2 sin(theta/2))

radius <- function(theta)	# assuming chord length is 1 px
{
	return(1/(2*sin(theta/2)))
}

# try some angles (SVG works in degrees, so I'll use degrees)
