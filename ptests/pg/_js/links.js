function accessLink(nNeigh, recuo)
{
    
    if( recuo==1 )
	path = "../"
    else if( recuo==2 )
	path = "../../"
    else if( recuo==3 )
	path = "../../../"
    else 
	path = "../../../../"
  
    if( nNeigh==1 )
      location.href= path+"about_neigh/isj.html"
      
    if( nNeigh==2 )
      location.href= path+"about_neigh/oj.html"
      
    if( nNeigh==3 )
      location.href= path+"about_neigh/stj.html"
      
    if( nNeigh==4 )
      location.href= path+"about_neigh/op.html"
      
    if( nNeigh==5 )
      location.href= path+"about_neigh/sctp.html"
      
    if( nNeigh==6 )
      location.href= path+"about_neigh/cpp.html"
    
    if( nNeigh==7 )
      location.href= path+"about_neigh/com.html"
      
    if( nNeigh==8 )
      location.href= path+"about_neigh/ctm.html"
      
    if( nNeigh==9 )
      location.href= path+"about_neigh/cthm.html"
      
    if( nNeigh==10 )
      location.href= path+"about_neigh/cfm.html"
      
    if( nNeigh==11 )
      location.href= path+"about_neigh/ssjw.html"
      
    if( nNeigh==12 )
      location.href= path+"about_neigh/sijw.html"
      
    if( nNeigh==13 )
      location.href= path+"about_neigh/cpe.html"
      
    if( nNeigh==14 )
      location.href= path+"about_neigh/csp.html"
  
}

function detailsAccessLink(algoritmo, test)
{
    path = "../" + algoritmo + "/test"
    
    if( test < 10)
	ret = path + "00" + test.toString() + "/test00" + test.toString() + "_details.html"
    else if( test < 100)
	ret = path + "0" + test.toString() + "/test0" + test.toString() + "_details.html"
    else
	ret = path + "" + test.toString() + "/test" + test.toString() + "_details.html"
    
    location.href=ret
      
}

function analysisAccessLink(algoritmo, test)
{
    path = "../" + algoritmo + "/test"
    
    if( test < 10)
	ret = path + "00" + test.toString() + "/test00" + test.toString() + "_param.html"
    else if( test < 100)
	ret = path + "0" + test.toString() + "/test0" + test.toString() + "_param.html"
    else
	ret = path + "" + test.toString() + "/test" + test.toString() + "_param.html"
    
    location.href=ret
      
}

function resultsAccessLink(algoritmo, test)
{
  
    path = "../" + algoritmo + "/test"
    
    if( test < 10)
	ret = path + "00" + test.toString() + "/test00" + test.toString() + ".html"
    else if( test < 100)
	ret = path + "0" + test.toString() + "/test0" + test.toString() + ".html"
    else
	ret = path + "" + test.toString() + "/test" + test.toString() + ".html"
    
    location.href=ret
    
}

function improvNeighAccessLink(test)
{
    if( test < 10)
	location.href="../pg/neigh_analysis/improv_neigh_00"+test.toString()+".html" 
    else if( test < 100)
	location.href="../pg/neigh_analysis/improv_neigh_0"+test.toString()+".html" 
    else
	location.href="../pg/neigh_analysis/improv_neigh_"+test.toString()+".html" 
}
