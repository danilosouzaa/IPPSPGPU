/*
 * A very simple script to filter a table according to search criteria
 *
 * Author: Emmanuel Charpentier
 * http://leparlement.org/filterTable
 * See also http://www.vonloesch.de/node/23
 *
 * Author: Artur Gołdyn
 *
 * Released under GNU Lesser General Public Licence v3.0
 * https://github.com/Owidiusz/filterTable.js
 */
function filterTables(pairs){
    for (var pair_number = 0; pair_number < pairs.length; pair_number++) {
        var new_terms=pairs[pair_number][0].value.toLowerCase();
        if(new_terms != pairs[pair_number][0].filterTable_old_terms){ // only if something changed
            pairs[pair_number][0].filterTable_old_terms=new_terms; 							//remember the old...
            filterTable(pairs[pair_number]);					//and filter table!
        }
    }
}


function filterTable(data_set) {
    term=data_set[0]
    table=data_set[1]
    var fields_ids=data_set[3];

    dehighlight(table);
    var terms = term.value.toLowerCase().split(" ");

    if(data_set[2]){
        for (var r = 2; r < table.rows.length; r++) {
            var display = '';
            for (var i = 0; i < terms.length; i++) {
                if (table.rows[r].innerHTML.replace(/<[^>]+>/g, "").toLowerCase()
                    .indexOf(terms[i]) < 0) {
                    display = 'none';
                } else {
                    if (terms[i].length) highlight(terms[i], table.rows[r]);
                }
                table.rows[r].style.display = display;
            }
        }
    }else{
        for (var r = 1; r < table.rows.length; r++) {
            var display = 'none';
            for (var i = 0; i < terms.length; i++) {
                if(terms[i].length==0){
                    display='';
                } else {
                    for (var j = 0; j < fields_ids.length; j++) {
                        if (table.rows[r].cells.namedItem(fields_ids[j]).innerHTML.replace(/<[^>]+>/g, "").toLowerCase()
                            .indexOf(terms[i]) >= 0) {
                            display = '';
                            console.log('Terms: '+terms);
                            console.log('Terms.length: '+terms.length);
                            console.log('Wiersz: '+r+' Słowo: '+terms+','+i+' Pole: '+fields_ids[j]);
                            var start_time_micro = new Date().getTime();
                            highlight(terms[i], table.rows[r].cells.namedItem(fields_ids[j]));
                            var elapsed_time_micro = new Date().getTime() - start_time_micro;
                            console.log('Czas zakonczenia: '+elapsed_time_micro);
                        }
                    }//for var j = 0; ....
                }//...else...
            }//for var i = 0; ....
            table.rows[r].style.display = display;
        }//for var r = 1;....
    }


}

function dehighlight(container) {
    for (var i = 0; i < container.childNodes.length; i++) {
        var node = container.childNodes[i];

        if (node.attributes && node.attributes['class']
            && node.attributes['class'].value == 'highlighted') {
            node.parentNode.parentNode.replaceChild(
                document.createTextNode(
                    node.parentNode.innerHTML.replace(/<[^>]+>/g, "")),
                node.parentNode);
            // Stop here and process next parent
            return;
        } else if (node.nodeType != 3) {
            // Keep going onto other elements
            dehighlight(node);
        }
    }
}

/*
 * Create a
 * <span>preText <span class="highlighted">term</span> postText</span>
 * around each search term
 */
function highlight(term, container) {
    for (var i = 0; i < container.childNodes.length; i++) {
        var node = container.childNodes[i];
        if (node.nodeType == 3) {
            // Text node
            var data = node.data;
            var data_low = data.toLowerCase();
            if (data_low.indexOf(term) >= 0) {
                //term found!
                var new_node = document.createElement('span');

                node.parentNode.replaceChild(new_node, node);

                var result;
                while ((result = data_low.indexOf(term)) != -1) {
                    new_node.appendChild(document.createTextNode(
                        data.substr(0, result)));
                    new_node.appendChild(create_node(
                        document.createTextNode(data.substr(
                            result, term.length))));
                    data = data.substr(result + term.length);
                    data_low = data_low.substr(result + term.length);
                }
                new_node.appendChild(document.createTextNode(data));
            }
        } else {
            // Keep going onto other elements
            highlight(term, node);
        }
    }
}

function create_node(child) {
    var node = document.createElement('span');
    node.setAttribute('class', 'highlighted');
    node.attributes['class'].value = 'highlighted';
    node.appendChild(child);
    return node;
}



/*
 * Here is the code used to set a filter on all filterable elements, usually I
 * use the behaviour.js library which does that just fine
 */

var inputs_elements_pairs = new Array();
tables = document.getElementsByTagName('table');
for (var t = 0; t < tables.length; t++) {
    element = tables[t];

    if (element.attributes['class']
        && element.attributes['class'].value.indexOf('filterable') != -1 ) {

        var filterTable_js_language_phrase_filtering="Filtering";
        var filterTable_js_language_phrase_all_fields="All Fields";
        var filterTable_js_language_phrase_no_fields='No fields';
        var filterTable_js_language_phrase_N_fields='fields';

        /* Description: */
        var filter=document.createElement('span');
        filter.setAttribute('class', 'filter');
        filter.innerHTML=filterTable_js_language_phrase_filtering;
        /* Here is dynamically created a form */
        var form = document.createElement('form');
        form.setAttribute('class', '_');
        // For ie...
        form.attributes['class'].value = 'filter';
        var input = document.createElement('input');
        input.setAttribute('id', 'filter-'+t);
        inputs_elements_pairs.push([input,element,true,'']);
        form.appendChild(input);
        /*Fields selector*/
        var filter_fields=document.createElement('button');
        filter_fields.setAttribute('class', 'filter-fields');
        filter_fields.setAttribute('id','filter-fields-'+t);
        filter_fields.setAttribute('button', 'button');
        filter_fields.setAttribute('onclick', "showDiv('filter-fields-list-"+t+"')");
        filter_fields.innerHTML=filterTable_js_language_phrase_all_fields;
        /* TODO: Fields selector list with checkboxes */
        var filter_fields_list=document.createElement('div');
        filter_fields_list.setAttribute('class', 'filter-fields-list');
        filter_fields_list.setAttribute('id', 'filter-fields-list-'+t);
        //calculate column length
        var columns_to_make=3;
        var column_length=(element.rows[0].children.length-element.rows[0].children.length%columns_to_make)/columns_to_make+1;

        var selector_html='<div class="select_field select_all"><input type="checkbox" name="__all_fields__" value="__all_fields__" onclick="processCheckBox(\'filter-fields-list-'+t+'\')" checked="checked"/><div class="select_field_description">'+filterTable_js_language_phrase_all_fields+'</div></div>';
        selector_html+='<div class="select_column">';
        for (var header_number = 0; header_number < element.rows[0].children.length; header_number++){
            if(header_number>0 && header_number%column_length==0){
                selector_html+='</div><div class="select_column">';
            }
            selector_html+='<div class="select_field" >';
            selector_html+='<input type="checkbox" name="'+element.rows[0].children[header_number].getAttribute('id')+'" value="'+element.rows[0].children[header_number].getAttribute('id')+'" onclick="processCheckboxes(\'filter-fields-list-'+t+'\')" checked="checked" title="'+element.rows[0].children[header_number].children[0].innerHTML+'" />';
            selector_html+='<div class="select_field_description" title="'+element.rows[0].children[header_number].children[0].innerHTML+'">'+element.rows[0].children[header_number].children[0].innerHTML+"</div>";
            selector_html+='</div>';
        }
        selector_html+='</div>';


        filter_fields_list.innerHTML=selector_html;


        /* Grouping div */
        var filter_container=document.createElement('div');
        filter_container.setAttribute('class','filter_container');
        filter_container.appendChild(filter);
        filter_container.appendChild(form);
        filter_container.appendChild(filter_fields);
        filter_container.appendChild(filter_fields_list);
        /*writing to document */
        element.parentNode.insertBefore(filter_container, element);
    }
}

onkeyup = function() {
    filterTables(inputs_elements_pairs);
}

function showDiv(id) {
    if(document.getElementById(id).style.display=='block'){
        document.getElementById(id).style.display='none';
    }else{
        document.getElementById(id).style.display = "block";
    }
}

function processCheckBox(filterFieldsListId){
    var AllChecked=document.getElementById(filterFieldsListId).children[0].children[0].checked;
    for (var div_select_col = 1; div_select_col < document.getElementById(filterFieldsListId).children.length; div_select_col++){
        for (var div_select_field = 0; div_select_field < document.getElementById(filterFieldsListId).children[div_select_col].children.length; div_select_field++){
            if(AllChecked){
                document.getElementById(filterFieldsListId).children[div_select_col].children[div_select_field].children[0].checked=true;
            }else{
                document.getElementById(filterFieldsListId).children[div_select_col].children[div_select_field].children[0].checked=false;
            }
        }//end for(var div_select_field....
    }//end for(var div_select_col...
    processCheckboxes(filterFieldsListId);
}

function processCheckboxes(filterFieldsListId){
    var selectedFieldsList=new Array();
    var count_all_fields=0;
    var count_checked_fields=0;
    for (var div_select_col = 1; div_select_col < document.getElementById(filterFieldsListId).children.length; div_select_col++){
        for (var div_select_field = 0; div_select_field < document.getElementById(filterFieldsListId).children[div_select_col].children.length; div_select_field++){
            count_all_fields=count_all_fields+1;
            if(document.getElementById(filterFieldsListId).children[div_select_col].children[div_select_field].children[0].checked){
                selectedFieldsList.push(document.getElementById(filterFieldsListId).children[div_select_col].children[div_select_field].children[0].value);
                count_checked_fields=count_checked_fields+1;
            }
        }//end for(var div_select_field....
    }//end for(var div_select_col...
    if(count_all_fields==count_checked_fields){
        var AllChecked=true;
        document.getElementById(filterFieldsListId).children[0].children[0].checked=true;
    }else{
        var AllChecked=false;
        document.getElementById(filterFieldsListId).children[0].children[0].checked=false;
    }

    var TableNumber=filterFieldsListId.split("-");
    TableNumber=TableNumber[TableNumber.length-1];
    if(AllChecked){
        document.getElementById('filter-fields-'+TableNumber).innerHTML=filterTable_js_language_phrase_all_fields;
    }else if(selectedFieldsList.length==0){
        document.getElementById('filter-fields-'+TableNumber).innerHTML=filterTable_js_language_phrase_no_fields;
    }else if(selectedFieldsList.length==1){
        document.getElementById('filter-fields-'+TableNumber).innerHTML=selectedFieldsList[0];
    }else{
        document.getElementById('filter-fields-'+TableNumber).innerHTML=selectedFieldsList.length+' '+filterTable_js_language_phrase_N_fields;
    }
    var data_set_for_filter_function=inputs_elements_pairs[TableNumber-1];
    data_set_for_filter_function[2]=AllChecked;
    data_set_for_filter_function[3]=selectedFieldsList;
    inputs_elements_pairs[TableNumber-1]=data_set_for_filter_function;
    filterTable(data_set_for_filter_function);
}
