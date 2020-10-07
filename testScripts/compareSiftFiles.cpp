#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

const float M_PI2 = 2.0f * 3.14159265358979323846f;

typedef vector<float> desc_t;

struct feat_t
{
    float  x;
    float  y;
    float  sigma;
    float  ori;
    desc_t desc;

    feat_t( int num, const vector<float>& input );

    void print( ostream& ostr ) const;

    void compareBestMatch( const vector<feat_t>& r ) const;

private:
    float dist( const feat_t& r ) const;
};

int readFeats( vector<feat_t>& l_one, fstream& f_one );
bool addFeat( vector<feat_t>& features, char* line );
void usage( char* name );

int main( int argc, char* argv[] )
{
    if( argc != 3 ) usage( argv[0] );

    fstream f_one( argv[1], fstream::in );
    fstream f_two( argv[2], fstream::in );

    if( ! f_one.good() )
    {
        cerr << "File " << argv[1] << " is not open." << endl;
        exit( -1 );
    }
    if( ! f_two.good() )
    {
        cerr << "File " << argv[2] << " is not open." << endl;
        exit( -1 );
    }

    vector<feat_t> l_one;
    vector<feat_t> l_two;

    int lines_read;

    lines_read = readFeats( l_one, f_one );
    cerr << "Read " << lines_read << " lines from " << argv[1] << endl;
    lines_read = readFeats( l_two, f_two );
    cerr << "Read " << lines_read << " lines from " << argv[2] << endl;

#if 0
    for( auto l : l_one )
    {
        l.print( cout );
        cout << endl;
    }
    for( auto l : l_two )
    {
        l.print( cout );
        cout << endl;
    }
#endif

    for( auto l : l_one )
    {
        l.compareBestMatch( l_two );
    }
}

void usage( char* name )
{
    cerr << "Usage: " << name << " <descriptorfile> <descriptorfile>" << endl
         << "       compute the L1 and L2 distance between the descriptors of" << endl
         << "       closest coordinate pairs. When a coordinate has 2 several" << endl
         << "       orientations, the closest distance is reported." << endl
         << "       Summary information at the end." << endl
         << endl;
    exit( 0 );
}

int readFeats( vector<feat_t>& l_one, fstream& f_one )
{
    char buffer[1024];
    int  lines_read;
    
    lines_read = 0;
    while( f_one.good() )
    {
        f_one.getline( buffer, 1024 );
        if( f_one.good() )
        {
            bool success = addFeat( l_one, buffer );
            if( success )
            {
                lines_read++;
            }
        }
    }
    return lines_read;
}

bool addFeat( vector<feat_t>& features, char* line )
{
    vector<float> values(5+128); // 4 or 5 values followed by 128 desc values

    int i = 0;
    istringstream s( line );
    while( s >> values[i] )
    {
        i++;
    }

    cerr << "Found " << i << " floats in line" << endl;
    features.emplace_back( i, values );

    if( i == 0 ) return false;
    return true;
}

feat_t::feat_t( int num, const vector<float>& input )
    : desc(128)
{
    auto it = input.begin();
    auto to = desc.begin();
    if( num == 132 )
    {
        x     = *it++;
        y     = *it++;
        sigma = *it++;
        ori   = *it++;
        for( int i=0; i<128; i++ ) *to++ = *it++;
    }
    else if( num == 133 )
    {
        float odbss;
        x     = *it++;
        y     = *it++;
        odbss = *it++;
        sigma = odbss == 0.0f ? 0.0f : sqrtf( 1.0f / odbss );
        ori   = 0.0f;
        it++;
        it++;
        for( int i=0; i<128; i++ ) *to++ = *it++;
    }
    else
    {
        cerr << "The keypoint line contains an unexpected number of floats (" << num << ")" << endl;
        return;
    }
}

void feat_t::print( ostream& ostr ) const
{
    ostr << "(" << x << "," << y << ")";
    ostr << " sigma=" << sigma << " ori=" << ori;
    for( auto it : desc )
    {
        ostr << " " << it;
    }
}

void feat_t::compareBestMatch( const vector<feat_t>& l_one ) const
{
    vector<float> distances;
    distances.reserve( l_one.size() );

    cout << "==========" << endl;
    for( auto r : l_one )
    {
        float v = dist( r );
        distances.push_back( v );
    }

    auto it = distances.begin();

    auto m = min_element( distances.begin(), distances.end() );

    for( auto r : l_one )
    {
#if 0
        if( it == m )
        {
            cout << "desc dist " << *it++
                 << " pixdist=" << sqrtf( (x-r.x)*(x-r.x) + (y-r.y)*(y-r.y) )
                 << " angledist=" << fabsf( ori/M_PI2*360.0f - r.ori/M_PI2*360.0f )
                 << endl;
        }
#else
        cout << "desc dist " << *it;
        if( it == m )
             cout << " MIN ";
        else
             cout << "     ";
        it++;
        cout << " pixdist=" << sqrtf( (x-r.x)*(x-r.x) + (y-r.y)*(y-r.y) )
             << " angledist=" << fabsf( ori/M_PI2*360.0f - r.ori/M_PI2*360.0f )
             << endl;
#endif
    }
}

float feat_t::dist( const feat_t& r ) const
{
    float sum = 0.0f;
    auto it_r = r.desc.begin();
    for( auto l : desc )
    {
        float val = l - *it_r++;
        sum += ( val * val );
    }
    return sqrtf( sum );
}

