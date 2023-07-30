#!/usr/bin/Rscript
# This file uses R to plot the competition_*.tsv file output by my code
library(ggplot2)
filename = commandArgs(TRUE)[1]
ggplot(read.table(filename, header=T), aes(x, y, color=cut(fit, 5))) + geom_point() + coord_fixed() + scale_color_brewer()
