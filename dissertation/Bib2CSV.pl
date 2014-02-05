#!/usr/bin/perl

use strict;
use warnings;
use Text::BibTeX;
use Getopt::Long;

my ($csvfile,$y,$a);
GetOptions ('o=s' => \$csvfile, 'a' => \$a, 'y' => \$y);

# Prepare output file

# Read input BibTeX file
my $file = shift or die("Missing input file.\n");

# Load file into Text::BibTeX structure
my $bibfile = new Text::BibTeX::File "$file";

# Keyword array
my @keywords;

# CSV Map
my %map;

# Read all entries from file
while (my $entry = new Text::BibTeX::Entry $bibfile)
{
	# Safe guarding
  	warn "Syntax error in input file." unless $entry->parse_ok;

	# Read the citation key
	my $key = $entry->key;
	
	# Read the file keywords and store them in an array
	my $wordsWithRep = $entry->get('keywords');
	
	# convert all elements to lowercase
	my $wordsWithRepLc = lc($wordsWithRep) if defined ($wordsWithRep);
	my @words;
	@words = split /\s*,\s*/, $wordsWithRepLc if defined ($wordsWithRepLc);
		
	if($y){
		my $yearEntry = $entry->get('year');
		push @words,$yearEntry;
	}
	if($a){
		my @authorEntry = $entry->split ('author');
		push @words,@authorEntry;
	}
	
	# Store keywords information in a HashMap
	$map{$key} = [@words];
	
	# Join entry keywords with global structure keywords
	push @keywords, @words;
}

# Create a new keywords with no repeated keywords
my @keywordsNoRep = keys %{{map {$_=>0} @keywords}};
@keywordsNoRep = sort @keywordsNoRep;

# Write the header line
my $out = ",\"";
$out.=join "\",\"",@keywordsNoRep;
$out.="\",\n";

# Iterate over the HashMap and write one line per entry
foreach my $key ( keys %map) {
	
	# Start an empty line
	my $line="";
	
	# Add the entry key
	$line.="\"$key\",";
	
	# For each global keyword
	foreach my $global_keyword (@keywordsNoRep) {
		
		# Start with no connection
		my $hasConnection = 0;
		
		# For each entry keyword
		foreach my $entry_keyword ( @{$map{$key}} ){
			
			# Assert connection and change connection status
			if ( $entry_keyword eq $global_keyword ){
				$hasConnection = 1;
			}
		}
		
		# Add connection information to the line
		$line.="$hasConnection,";
	}
	
	$line.="\n";
	$out.=$line;
}

# Write out the csv file
if (defined $csvfile){
	open (FOUT,">$csvfile");
	print FOUT $out;
	close FOUT;
} else {
	print $out;
}

__END__

=head1 NAME

bib2csv - create CSV file for concept analysys from BibTeX keyword and year attributes.

=head1 SYNOPSIS

bib2csv [B<-ya>] [-o F<filename>] F<bibfile.bib>

=head1 DESCRIPTION

This tool converts a LaTeX B<bibliography> file (B<BibTeX>) annotated with B<keywords> into a B<csv> file for use with the I<ConExp> tool available at I<http://conexp.sourceforge.net/>.

=head1 OPTIONS

=over 2
	
=item B<-o> F<filename>

outputs to the specified file.
	
=item B<-y> 

use information about the year of the bib entry for the attributes in addition to the keywords.

=item B<-a> 

use information about the author(s) of the entry for the attributes in addition to hte keywords.

=back


=head1 AUTHORS

Marcio Coelho and Tiago Veloso.

=head1 COPYRIGHT

Permission is granted to copy, distribute and/or modify this 
document under the terms of the GNU Public Licence.

=cut