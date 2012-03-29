/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

(function ($) {
    var currentIndex = 0;
    var totalItems = 0;
    
    Drupal.behaviors.advpollModule = {
        attach: function (context, settings) {
            // only rebuild if it doesn't exist - ajax calls/validation require this to be refreshed now and then'
            if ($('ul.selectable-list').length < 1) {
                currentIndex = 0;
                //  $('.advpoll-ranking-wrapper select').css('display', 'none');
                $('.advpoll-ranking-wrapper input.form-text').css('display', 'none');
                $('.advpoll-ranking-wrapper label').append('<a href="" class="vote add">'+Drupal.t('Add')+' ></a>');
                $('.advpoll-ranking-wrapper .form-type-select').wrap('<li class="selectable" />');
                $('.advpoll-ranking-wrapper .form-type-textfield').wrap('<li class="selectable" />');
                $('.advpoll-ranking-wrapper li').wrapAll('<ul class="selectable-list" />');
                $('#advpolltable').append('<tfoot><tr><td></td></tr></tfoot>');
                $('#advpolltable tfoot tr td').append($('.advpoll-ranking-wrapper .form-submit'));
                $('.advpoll-ranking-wrapper li a').each(function(index){
                    $(this).data('index', index);
                });                              
                
                totalItems = $('#advpolltable tbody tr').length;
                
                $('.advpoll-ranking-wrapper ul.selectable-list li').each(function(index) {
                    if($(this).find('select').length) {
                        var select = $(this).find('select');
                        var selected = $("#"+select.attr('id')+" option[selected = selected]");
                        
                        if (selected.length) {
                            if (selected.val() > 0) {
                                var element = $(selected).parents('li');
                                $(element).find('a.vote').removeClass('add').addClass('remove').html('(x)');
                                $('#advpolltable tbody td').eq(selected.val()-1).append(element);
                                $('#advpolltable tbody tr').eq(selected.val()-1).css('display', 'block');
                                currentIndex++;
                            }
                        }
                    }
                });
                
                
                Drupal.advpollUpdateEvents();

            }      

            if (Drupal.tableDrag) {
                tableDrag = Drupal.tableDrag.advpolltable;
              

                // Add a handler for when a row is swapped.
                tableDrag.row.prototype.onSwap = function (swappedRow) {

                };

                // Add a handler so when a row is dropped, update fields dropped into new regions.
                tableDrag.onDrop = function () {                    
                    Drupal.advpollUpdateSelect();
                    return true;
                };

            }
            
        }
        
        
        
       
    };
    
    Drupal.advpollUpdateEvents =  function () {
        Drupal.advpollRemoveEvents();
        $('.advpoll-ranking-wrapper ul.selectable-list li a.add').bind('click', function(){
            var element = $(this).parents('li');
            $(this).removeClass('add').addClass('remove').html('(x)');
            console.log(currentIndex);
            $('#advpolltable tbody td').eq(currentIndex).append(element);
            $('#advpolltable tbody tr').eq(currentIndex).css('display', 'block');
            Drupal.advpollUpdateEvents();
            currentIndex++;
            return false;
        });


        $('.advpoll-ranking-wrapper td a.remove').bind('click', function(){
            var element = $(this).parents('li');
            var select = element.find('select');
            $('.advpoll-ranking-wrapper ul.selectable-list').append(element);
            $(this).removeClass('remove').addClass('add').html(Drupal.t('Add >'));
            
            $("#"+select.attr('id')+" option[value='0']").attr('selected', 'selected');
            currentIndex--;
            Drupal.advpollReorderChoices();
            Drupal.advpollUpdateEvents();
            return false;
        });
        
        
    }
       
    Drupal.advpollReorderChoices = function() {
        var choices = [];
        
        $('.advpoll-ranking-wrapper tr li').each(function() {
            choices.push($(this));
        });
       
        for (var i = 0; i < choices.length; i++) {
            $('#advpolltable tbody td').eq(i).append(choices[i]);
        }
    }
       
    Drupal.advpollRemoveEvents =  function () {
        $('.advpoll-ranking-wrapper ul.selectable-list li a.add').unbind('click');
        $('.advpoll-ranking-wrapper td a.remove').unbind('click');
        Drupal.advpollUpdateSelect();
    }
   
    Drupal.advpollUpdateSelect = function() {
        $('#advpolltable tbody tr').each(function(index) {
            if ($(this).find('select').length) {
                $(this).css('display', 'block');

                var select = $(this).find('select');
                $("#"+select.attr('id')+" option[value='"+(index + 1)+"']").attr('selected', 'selected');
                
            } else if ($(this).find('input').length) {
                $(this).css('display', 'block');
                $(this).find('input').css('display', 'block');
                $("input[name='write_in_weight']").attr('value', (index+1));
            } else {
                $(this).css('display', 'none');
            }
        });
        
        if ($('.advpoll-ranking-wrapper ul.selectable-list li input').length) {
            $('.advpoll-ranking-wrapper ul.selectable-list li input').css('display', 'none');
            $("input[name='write_in_weight']").attr('value', 0);
        }
    }
   
    /**
   * Get rid of irritating tabledrag messages
   */
    Drupal.theme.tableDragChangedWarning = function () {
        return [];
    }
  
    Drupal.theme.prototype.tableDragIndentation = function () {
        return [];
    };

    Drupal.theme.prototype.tableDragChangedMarker = function () {
        return [];
    };   

})(jQuery);