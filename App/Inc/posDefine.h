/*
 * posDefine.h
 *
 *  Created on: 2021年2月27日
 *      Author: Annew
 */

#ifndef INC_POSDEFINE_H_
#define INC_POSDEFINE_H_

// define the board x,y
#define VLINE0_X	0
#define VLINE1_X	49
#define VLINE1_USER_X   180// 207
#define VLINE2_X	98
#define HLINE1_X	0
#define HLINE1_Y	15
#define HLINE2_X	98
#define HLINE2_Y	27
#define HLINE3_X	98
#define HLINE3_Y	39

#define VLINE1_USER_SUB_X	 80
#define SUB_X3_X    30
#define  TITLE_FILTER_NAME_USER_X    195





#define VLINE_USER1_X      207
#define HLINE_USER3_Y      78

#define TITLE_UNION_SECOND_X   10
#define SPOT_NUM_X             90
#define SIDE_USER_X            50
#define FILTER_NAME_X    185 



//#define HLINE4_X	107
//#define HLINE4_Y	51
#define HLINE_LEN	(256-VLINE2_X)

#define WIDTH_LIGHT		(VLINE1_X-VLINE0_X)
#define WIDTH_FILTER	(VLINE2_X-VLINE1_X)
#define WIDTH_UNION		(0xff-VLINE2_X)
#define WIDTH_UNION_SMART   (0xff-TITLE_UNION_SECOND_X) 

#define WIDTH_USER_LIGHT	(VLINE1_USER_X -VLINE0_X)
#define WIDTH_USER_FILTER	(VLINE2_X-VLINE1_X)
#define WIDTH_USER_UNION		(0xff-VLINE2_X)
#define WIDTH_UNION_USER   (0xff-TITLE_UNION_SECOND_X)




#define TITLE_LIGHT_X		VLINE0_X
#define TITLE_LIGHT_Y		0
#define TITLE_LIGHT_HEIGHT	HLINE1_Y

//User Defined Mode 
#define TITLE_LIGHT_USER_X				VLINE1_USER_X //VLINE0_X
#define TITLE_LIGHT_USER_Y				HLINE3_Y //0
#define TITLE_LIGHT_USER_HEIGHT			HLINE1_Y

//User Defined Mode 'fiter'
#define TITLE_FILTER_USER_X				TITLE_FILTER_NAME_USER_X //VLINE_USER1_X //VLINE1_X
#define TITLE_FILTER_USER_Y		    	HLINE1_Y//0
#define TITLE_FILTER_USER_HEIGHT		HLINE1_Y


#define LIGHT_INFO_USER_X		VLINE1_USER_X	 
#define LIGHT_INFO_USER_Y		HLINE1_Y
#define LIGHT_INFO_USER_HEIGHT	(HLINE3_Y-HLINE1_Y)


#define LIGHT_SUBINFO_USER_X		   VLINE1_USER_SUB_X	
#define LIGHT_SUBINFO_USER_Y		   HLINE1_Y
#define LIGHT_SUBINFO_USER_HEIGHT	   (HLINE3_Y-HLINE1_Y)

#define LIGHT_INFO_NUM_X            
#define LIGHT_INFO_NUM_Y


#define UNION_INFO3_X		TITLE_UNION_X
#define UNION_INFO3_Y		HLINE3_Y
#define UNION_INFO3_HEIGHT	(63-HLINE3_Y)

#define UNION_INFO3_SUB_X		      SUB_X3_X  //TITLE_UNION_X
#define UNION_INFO3_SUB_Y		      HLINE3_Y
#define UNION_INFO3_SUB_HEIGHT	      (63-HLINE3_Y)

#define LIGHT_NUM_SPOT_X				SPOT_NUM_X
#define LIGHT_NUM_SPOT_Y				HLINE3_Y
#define LIGHT_NUM_SOPOT_HEIGHT			(63-HLINE3_Y)


#define LIGHT_SIDE_USER_X		   SIDE_USER_X
#define LIGHT_SIDE_USER_Y		   HLINE1_Y
#define LIGHT_SIDE_USER_HEIGHT	   (HLINE3_Y-HLINE1_Y)



/********************************Smart Mode********************************************/
//"Filter"
#define TITLE_FILTER_X		VLINE1_X
#define TITLE_FILTER_Y		0
#define TITLE_FILTER_HEIGHT	(HLINE3_Y-HLINE1_Y)
//'Smart Mode' 
#define TITLE_UNION_X		VLINE1_X//VLINE2_X //WT.EDIT 
#define TITLE_UNION_Y		0
#define TITLE_UNION_HEIGHT	HLINE1_Y

#define LIGHT_INFO_X		TITLE_LIGHT_X
#define LIGHT_INFO_Y		HLINE1_Y
#define LIGHT_INFO_HEIGHT	(HLINE3_Y-HLINE1_Y)

#define LIGHT_NUM_X			TITLE_LIGHT_X
#define LIGHT_NUM_Y			HLINE3_Y
#define LIGHT_NUM_HEIGHT	(63-HLINE3_Y)

#define FILTER_INFO_X		TITLE_FILTER_X
#define FILTER_INFO_Y		HLINE1_Y
#define FILTER_INFO_HEIGHT	(HLINE3_Y-HLINE1_Y)

#define FILTER_INFO_USER_X				TITLE_FILTER_X
#define FILTER_INFO_USER_Y				HLINE1_Y
#define FILTER_INFO_USER_HEIGHT			(HLINE3_Y-HLINE1_Y)



#define FILTER_NUM_X		TITLE_FILTER_X
#define FILTER_NUM_Y		HLINE3_Y
#define FILTER_NUM_HEIGHT	(63-HLINE3_Y)

//The user defined Mode "filter Name"
#define FILTER_NUM_USER_X		VLINE1_USER_X//TITLE_FILTER_X
#define FILTER_NUM_USER_Y		HLINE3_Y
#define FILTER_NUM_USER_HEIGHT	(63-HLINE3_Y)

#define FILTER_NAME_USER_X		FILTER_NAME_X//TITLE_FILTER_X
#define FILTER_NAME_USER_Y		HLINE3_Y
#define FILTER_NAME_USER_HEIGHT	(63-HLINE3_Y)



//The second line 
#define UNION_INFO1_X		TITLE_UNION_SECOND_X //TITLE_UNION_X
#define UNION_INFO1_Y		HLINE1_Y
#define UNION_INFO1_HEIGHT	(HLINE2_Y-HLINE1_Y)
//The third line
#define UNION_INFO2_X		TITLE_UNION_SECOND_X //TITLE_UNION_X
#define UNION_INFO2_Y		HLINE2_Y
#define UNION_INFO2_HEIGHT	(HLINE3_Y-HLINE2_Y)

#define UNION_INFO3_X		TITLE_UNION_X
#define UNION_INFO3_Y		HLINE3_Y
#define UNION_INFO3_HEIGHT	(63-HLINE3_Y)

// define the position of Filter Index
#define FILTER_INDEX_X		75
#define FILTER_INDEX_Y		62
#define MAX_FILTER_INDEX	10
#define MAX_FILTER_STR_LEN	 5

#define MAX_FILTER_STR_SUB_LEN	 15//10


// define the position of Light Index
#define LIGHT_INDEX_X				17
#define LIGHT_INDEX_Y				62
#define MAX_LIGHT_INDEX				16//WT.EDIT //10
#define MAX_LIGHT_STR_LEN			60//6
#define MAX_LIGHT_LINEAR_STR_LEN        8
#define  MAX_LIGHT_SPOT_INDEX       6     
#define  MAX_LIGHT_LINEAR_INDEX     4

//define the sopt board of right and left 
#define MAX_LIGHT_LR_INDEX		   5  //WT.EDIT 2021.04.23
#define MAX_LIGHT_STR_LR_LEN      200  //  6  //wt.EDIT 2021.04.23


#define UNION_STR_X				136
#define UNION_STR_Y				62
#define MAX_UNION_INDEX			21//20 //WT.EDIT 2021.05.08
#define MAX_UNION_MAIN_INDEX    5  //wt.EDIT 2021.05.08
#define MAX_UNION_INFO1_STR_LEN	 38 //26//24 //WT.EDIT 2021.06.18 char number
#define MAX_UNION_INFO2_STR_LEN	26//25
#define MAX_UNION_STR_LEN	(MAX_FILTER_STR_LEN+MAX_LIGHT_STR_LEN+5)

#endif /* INC_POSDEFINE_H_ */
