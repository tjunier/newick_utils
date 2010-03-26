#!/usr/bin/env ruby

# test the scale algorithm

number = ARGV.first.to_f

nearest_smaller_power_of_ten = Math.log10(number).floor
nearest_greater_power_of_ten = nearest_smaller_power_of_ten + 1

low_PoT = 10 ** nearest_smaller_power_of_ten
high_PoT = 10 ** nearest_greater_power_of_ten

brackets = [low_PoT, low_PoT / 2.0, low_PoT / 4.0, low_PoT / 5.0]
brackets.concat [high_PoT, high_PoT / 2.0, high_PoT / 4.0, high_PoT / 5.0]

desired_tick_numbers = [4, 5]

solutions = []
brackets.each do |br|
	ticks = (number / br).floor
	error = number - (ticks * br)
	rel_error = (error / number).abs
	diffs = desired_tick_numbers.map {|n| (ticks - n).abs}
	min_diff = diffs.min
	score = 10 * min_diff + 100 * rel_error
	solutions << [ticks, br, error, rel_error, score]
end

solutions.sort! {|a,b| a[4] <=> b[4]}
solutions.each {|sol| p sol}

