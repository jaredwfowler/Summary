/*Created By: Jared Fowler
June 2014
Math 482, Prof. John Dye


A common question includes finding a specific kth smallest node's
max/min height and depth.  An algorithm to solve this is presented below.*/



#include <iostream>
#include <cmath>
using namespace std;


int main(){

    //Variables to be changed
    int k = 4;
    int n = 1111;


    //Do not modify below this line!
    //Other Variables
    int max_height = -1;
    int min_height = -1;
    int max_depth = -1;
    int min_depth = -1;

    int lg_k = log(k) / log(2);	//lg_2(k)
    int lg_n = log(n) / log(2);	//lg_2(n)

    int last_row_leaves = n - (pow(2, lg_n) - 1);	//Number of used leaves on the last row
    int completeBoost = 0; //Used in the situation that k has a complete tree under it.
    

    //Min Height: Same Simple Case For All! 
    /*This is really determined by how many nodes are
    greater than the kth value.  Imagine following the left most branch.
    The only factor that would limit a nodes min height is if there wasn't
    enought nodes greater than it to get back to the root*/
    int numberLarger = n - k;
    min_height = lg_n - numberLarger;
    if (min_height < 0)
        min_height = 0;

    //Max Depth is just as easy to find.  We subtract the tree height from min height
    max_depth = lg_n - min_height;

#if(0) //Uses case 3
    //*****************************************************************************
    /*Case 1: K is the parent of a complete tree (sub-tree)
    This is probably the easiest case... */

    //Step 1A: Determine if there is a complete tree under k
    int lg_k_ciel = (log(k + 1) / log(2)) + .999999;
    if ((lg_k+1) == lg_k_ciel){





    }
#endif


    //We do need to take into consideration a k node with a complete tree under it.
    int matcher = (pow(2, lg_k + 1) - 1);
    if (k == matcher){
        completeBoost = 1;

    }



    //*****************************************************************************
    /*Case 2: K is the root of the entire tree.*/

    if (k == n){

        min_height = lg_n;
        max_height = lg_n;
        min_depth = 0;
        max_depth = 0;

    }

    //*****************************************************************************
    /*Case 3: K is neither of the two cases above.  This is the most
    difficult case, as we need to consider k's height dipping into the
    last row of leaves that may be incomplete!*/

    else{
        

        /*Our first objective, and probably the most difficult, is to find the max height.
        There are only two possible answers: a) floor(lg k) or b) floor(lg k) - 1  .  
        To obtain the first answer, we need to dip down into the last, incomplete row. We need
        to be careful, however, because we just can't pick and choose which nodes we want to 
        include... For example, say we wanted the last two nodes from the bottom row of a heap that 
        consists of 5 nodes.  If you draw this out, you can easily see that you cannot include 
        the second to last node withouth including the entire tree!!!*/


        /*We need to find out how many nodes, if any, on the bottom row could be used towards k's height.
        Because we fill a binary heap from left to right, and becasue we want to include as few as possible nodes
        on the bottom row in order to produce the max height, we need to to, therefore, read the binary heap from
        right to left.  We know that k's bottom level could have a max of floor(lg k) nodes... ,so, let's mod the 
        bottom by this number and find out how many we are left with. "Leftovers"
        
        Note: Realize that each level of the binary tree can be split in half again and again into smaller leaf sets 
        of subtrees which roots are furthur up the tree.*/

        //Step 3A: Get the leftovers as described above.
        int leftovers = (last_row_leaves % (static_cast<int>(pow(2, lg_k))));
        //There is the possibility that this will return 0.. we don't want this, so we add back on full bottom of k
        if (leftovers == 0){
            leftovers += (static_cast<int>(pow(2, lg_k)));
        }

        /*Suprisingly, we can now find the max height with one last test.  If the number of leftovers exceeds what could
        be added to k's complete tree, at the second to last level, withouth surpassing k, than max height is option a,
        else, it's option b*/


        //Step 3B: Determine if we can use the leftovers
        if ((k - (static_cast<int>(pow(2,lg_k)) - 1)) >= leftovers){
            max_height = lg_k;

        }
        else{
            max_height = lg_k - 1;
        }


        /*Min Depth: This is really easy now that we've found the max height.  We need to determine if we used any of the leftovers
        to get to our max height.  If this is the case, we can just take the tree height minus max height.  If we were unable to use
        these values, all hope isn't lost yet!  Remember how we partitioned the last row by % 2^(lgk) ?  If there are enough nodes on the
        next level up that don't have children that could serve as a full base to tree of root k, then our min depth will also be
        treeheight - lgk .  Else, if neither of these cases hold, we are foced to have a full row on the very bottom and have (treeheight + 1) - lgk*/
    
        //Step 3C: Find the min depth
        //Do we have enough childless parents on second to last row?
        if ((pow(2, lg_n) - last_row_leaves) / (pow(2, lg_k)) > 1){
            min_depth = lg_n - lg_k -completeBoost;
        }

        else{
            min_depth = lg_n - max_height;
        }
    
    }

    cout << "Nodes: " << n << " , Value: " << k << "th smallest" << endl << endl;
    cout << "Max Height: " << max_height << endl;
    cout << "Min Height: " << min_height << endl;
    cout << "Max Depth:  " << max_depth << endl;
    cout << "Min Depth:  " << min_depth << endl;

    cout << endl << endl;


    return 0;
}