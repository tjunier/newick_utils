#!/usr/bin/perl -w

use Bio::TreeIO;

my $file = shift;
my $label = shift;

my $in = Bio::TreeIO->new(-format => 'newick',
			  -file => $file);
my $out = Bio::TreeIO->new(-format => 'newick');
while( my $t = $in->next_tree ){
     my ($a) = $t->find_node(-id =>$label);
     #$out->write_tree($t);
     $t->reroot($a);
     $out->write_tree($t);
}
