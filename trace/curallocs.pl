#!/usr/bin/perl -w

# Show current allocations from a trace file. If run over a trace file
# from a program which has exited, this will show memory leaks. If run
# over a trace file for a program which is currently running, this will
# show currently allocated space.
#
# By Richard W.M. Jones <rich@annexia.org>
#
# $Id: curallocs.pl,v 1.1 2001/02/08 12:51:35 rich Exp $

use strict;

my %pools = ();

my $lineno = 0;

while (<>)
  {
    $lineno++;

    s/[\n\r]+$//;

    if (/^([a-z_]+)\s+caller:\s+([0-9a-fx]+)\s+ptr1:\s+([0-9a-fx]+|\(nil\))\s+ptr2:\s+([0-9a-fx]+|\(nil\))\s+ptr3:\s+([0-9a-fx]+|\(nil\))\s+i1:\s+([0-9]+)\s*$/)
      {
	my $fn = $1;
	my $caller = $2;
	my $ptr1 = $3 ne '(nil)' ? $3 : 0;
	my $ptr2 = $4 ne '(nil)' ? $4 : 0;
	my $ptr3 = $5 ne '(nil)' ? $4 : 0;
	my $i1 = $6;

	if ($fn eq "new_pool")
	  {
	    die "new_pool: pool exists, line $lineno"
	      if exists $pools{$ptr1};

	    $pools{$ptr1} = {
			     creator => $caller,
			     pool => $ptr1,
			     children => {},
			     allocations => {}
			    };
	  }
	elsif ($fn eq "new_subpool")
	  {
	    die "new_subpool: pool exists, line $lineno"
	      if exists $pools{$ptr1};

	    $pools{$ptr1} = {
			     creator => $caller,
			     pool => $ptr1,
			     parent => $ptr2,
			     children => {},
			     allocations => {}
			    };
	    $pools{$ptr2}{children}{$ptr1} = 1;
	  }
	elsif ($fn eq "delete_pool")
	  {
	    if ($pools{$ptr1}{parent})
	      {
		delete $pools{$pools{$ptr1}{parent}}{children}{$ptr1};
	      }

	    remove_pool_recursively ($ptr1);
	  }
	elsif ($fn eq "pmalloc")
	  {
	    die "pmalloc: no pool $ptr1, line $lineno"
	      unless exists $pools{$ptr1};

	    $pools{$ptr1}{allocations}{$ptr2} = {
						 creator => $caller,
						 pool => $ptr1,
						 address => $ptr2,
						 size => $i1
						};
	  }
	elsif ($fn eq "prealloc")
	  {
	    die "prealloc: no pool $ptr1, line $lineno"
	      unless exists $pools{$ptr1};
	    die "prealloc: allocation already exists, line $lineno"
	      unless exists $pools{$ptr1}{allocations}{$ptr2};

	    # Delete the old allocation.
	    delete $pools{$ptr1}{allocations}{$ptr2};

	    $pools{$ptr1}{allocations}{$ptr3} = {
						 creator => $caller,
						 pool => $ptr1,
						 address => $ptr3,
						 size => $i1
						};
	  }
	else
	  {
	    die "unknown pool function traced: $fn, line $lineno";
	  }
      }
    else
      {
	print "$lineno: $_\n";
	die "cannot parse line";
      }
  }

if (keys %pools > 0) {
  show_pools ();
} else {
  print "No pools are currently allocated.\n";
}

exit 0;

sub remove_pool_recursively
  {
    my $pool = shift;
    local $_;

    die unless exists $pools{$pool};

    # Remove children first.
    foreach (keys %{$pools{$pool}{children}})
      {
	remove_pool_recursively ($_);
      }

    delete $pools{$pool};
  }

sub show_pools
  {
    local $_;

    foreach (keys %pools)
      {
	show_pool ($_);
      }
  }

sub show_pool
  {
    my $pool = shift;
    my $indent = shift || 0;
    local $_;

    my $sp = " " x $indent;

    print $sp, "pool $pool created by $pools{$pool}{creator}:\n";
    print $sp, "  number of direct allocations: ", 0 + keys %{$pools{$pool}{allocations}}, "\n";
    print $sp, "  number of children: ", 0 + keys %{$pools{$pool}{children}}, "\n";
    print $sp, "  allocations:\n";

    show_allocations ($pools{$pool}{allocations}, $indent);

    print $sp, "  children:\n";

    foreach (keys %{$pools{$pool}{children}})
      {
	show_pool ($_, $indent + 2);
      }
  }

sub show_allocations
  {
    my $allocations = shift;
    my $indent = shift || 0;
    local $_;

    foreach (keys %$allocations)
      {
	show_allocation ($allocations->{$_}, $indent);
      }
  }

sub show_allocation
  {
    my $allocation = shift;
    my $indent = shift || 0;
    local $_;

    my $sp = " " x $indent;

    print $sp, "    allocation $allocation->{address} created by $allocation->{creator} size $allocation->{size}\n";
  }
