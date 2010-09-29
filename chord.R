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
# function assuming 1 pixel. The length c of the chord is given by:
#
# c = r crd(theta)
#
# where crd(theta) = 2 sin(theta/2)
#
# so, for a given resolution c, we have
#
# r = c / crd(theta) = c / 2 sin(theta/2)

radius <- function(theta, c)	# assuming chord length is 1 px
{
	return(c/(2*sin(theta/2)))
}

# try some small angles (SVG works in degrees, so I'll use degrees). Each angle
# is 10 times wider than the previous one.

angles_deg <- 10 ** seq(-2.5,0,0.2)
angles_rad <- angles_deg / 360 * 2*pi
rad_2 <- radius(angles_rad, 2)
rad_1 <- radius(angles_rad, 1)
rad_0.5 <- radius(angles_rad, 0.5)

line.width <- 2
plot(angles_deg, rad_1, type='l', lwd=line.width, log='x',
	main='Radius as a f() of angle\nat various resolutions (chord lengths)',
	xlab='angle [°]', ylab='radius')
lines(angles_deg, rad_0.5, lwd=line.width, col='blue')
lines(angles_deg, rad_2, lwd=line.width, col='red')

legend("topright", legend=c(0.5, 1, 2), col=c('blue', 'black', 'red'),
	title='chord', lwd=line.width, inset=c(0.1,0.1))

