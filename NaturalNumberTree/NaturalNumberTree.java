

import java.util.LinkedList;
import java.util.List;

/**
 * Recursively maps a natural number to a rooted, un-oriented tree.
 * @author Pete Cappello
 */
public class NaturalNumberTree 
{
    private static int[] primes;
    
    /**
     * Fill the primes array with the first numPrimes prime numbers.
     * @param numPrimes the number of elements in primes
     */
    public static void setPrimesArray( int numPrimes )
    {
        primes = new int[ numPrimes ];
        primes[ 0 ] = 2;
        primes[ 1 ] = 3;
        for ( int number = 5, rank = 2; rank < numPrimes; number += 2 )
        {
            if ( isPrime( number ) )
            {
                primes[ rank++ ] = number;
            }
        }
    }
    
    private static boolean isPrime( int number )
    {
        for ( int rank = 1; primes[ rank ] <= Math.sqrt( number ); rank++ )
        {
            if ( number % primes[ rank ] == 0 )
            {
                return false;
            }
        }
        return true;
    }
    
    private final int naturalNumber;
    private final List<NaturalNumberTree> factorTrees = new LinkedList<>();
    
    /**
     * Construct the tree that corresponds to a particular natural number.
     * @param naturalNumber whose corresponding tree is being constructed 
     */
    NaturalNumberTree( int naturalNumber )
    { 
        this.naturalNumber = naturalNumber;
        for ( int rank = 0; primes[ rank ] <= naturalNumber && naturalNumber > 1; rank++ )
        {
            // for each prime factor, create a subtree for the prime factor's rank
            for ( ; naturalNumber % primes[ rank ] == 0; naturalNumber /= primes[ rank ] )
            {
                factorTrees.add( new NaturalNumberTree( rank + 1 ) );
            }
        }
    }
    
    @Override
    public String toString() { return new String( toString( "   ") ); }
    
    private StringBuilder toString( String pad )
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append( pad ).append( '\n' ).append( pad ).append( "Natural number tree: rank: ");
        stringBuilder.append( naturalNumber ).append( " ");
        stringBuilder.append( naturalNumber < primes.length ? primes[ naturalNumber - 1 ] : "" );
        stringBuilder.append( " Its factor trees are:" );
        for ( NaturalNumberTree factorTree : factorTrees )
        {
            stringBuilder.append( factorTree.toString( pad + "   ") );
        }
        return stringBuilder;
    }
    
    public static void main( String[] args )
    {
        NaturalNumberTree.setPrimesArray( 100 );
        for ( int naturalNumber = 1; naturalNumber <= 16; naturalNumber++ )
        {
            NaturalNumberTree naturalNumberTree = new NaturalNumberTree( naturalNumber );
            System.out.println("\n naturalNumber: " + naturalNumber + naturalNumberTree );
        }
        
        int naturalNumber = 399;
        NaturalNumberTree naturalNumberTree = new NaturalNumberTree( naturalNumber );
        System.out.println("\n naturalNumber: " + naturalNumber +  naturalNumberTree );
    }
}

