
function TuxedoCat_BobTheBuilder(meta_json, public_url)
{
    var TUXEDO_CAT_BUILDER_VERSION = "0.001";
    var TUXEDOCAT_EDITOR_OBJ_KEY_START = "TUXEDOCAT_EDITOR_OBJ_START";

    var EXTRA_ROWS = 4;
    var FUNCTION_TYPES = { BUILD:0, ROLLOUT:1, ROLLIN:2 };

    var meta = meta_json;
    var publicURL = public_url;

    var editorParentID;

    // --------------------------------------------------------------------------------------------

    /*
     * Builds the editor page for admin
    */

    this.Editor_RollOut = function (parentID)
    {
        editorParentID = parentID;

        // Build Option Buttons

        var htmlStr = "<br><br><br>";

        htmlStr += '<a id="editor_button_saveAll" style="margin-right:5px; float:left; background-color:green;" \
                       class="waves-effect waves-light btn">Save</a> \
                    <a id="editor_button_saveRestorePoint" style="margin-right:5px; float:left; background-color:green;" \
                       class="waves-effect waves-light btn">Save As Restore Point</a> \
                    <a id="editor_button_factoryReset" style="margin-right:5px; float:right; background-color:red;" \
                       class="waves-effect waves-light btn">Factory Reset</a> \
                    <a id="editor_button_restorePoint" style="margin-right:5px; float:right; background-color:orange;" \
                       class="waves-effect waves-light btn">Load Restore Point</a>';

        htmlStr += "<br><br><br><br><br>";
        $(parentID).append(htmlStr);

        // Set Option Button Handlers

        $("#editor_button_saveAll").click(function()
        {
            var jsonStr = rollIn(editorParentID);

            //Create a jSon object and insert info

            var message = {};
            message.jsonStr = jsonStr;
            message.operation = "save";

            // Submit via ajax

            $.ajax({
                type: "POST",
                cache: false,
                url: publicURL + '/admin/editor/postJsonFileOperation',
                data: message,
                dataType: "json",
                success: function(data)
                {
                    if (data['status'] == 'success')
                    {
                        console.log("SAVE SUCCESS");
                        location.reload();
                    }
                    else
                    {
                        console.log("SAVE FAIL");
                    }
                },
                error: function(data)
                {
                    console.log("SAVE FAIL - AJAX FAIL");
                },
            });
        });

        $("#editor_button_saveRestorePoint").click(function()
        {
            var jsonStr = rollIn(editorParentID);

            //Create a jSon object and insert info

            var message = {};
            message.jsonStr = jsonStr;
            message.operation = "save_restore";

            // Submit via ajax

            $.ajax({
                type: "POST",
                cache: false,
                url: publicURL + '/admin/editor/postJsonFileOperation',
                data: message,
                dataType: "json",
                success: function(data)
                {
                    if (data['status'] == 'success')
                    {
                        console.log("SAVE_RESTORE SUCCESS");
                        location.reload();
                    }
                    else
                    {
                        console.log("SAVE_RESTORE FAIL");
                    }
                },
                error: function(data)
                {
                    console.log("SAVE_RESTORE FAIL - AJAX FAIL");
                },
            });
        });

        $("#editor_button_factoryReset").click(function()
        {
            //Create a jSon object and insert info

            var message = {};
            message.operation = "factory_reset";

            // Submit via ajax

            $.ajax({
                type: "POST",
                cache: false,
                url: publicURL + '/admin/editor/postJsonFileOperation',
                data: message,
                dataType: "json",
                success: function(data)
                {
                    if (data['status'] == 'success')
                    {
                        console.log("FACTORY RESET SUCCESS");
                        location.reload();
                    }
                    else
                    {
                        console.log("FACTORY RESET FAIL");
                    }
                },
                error: function(data)
                {
                    console.log("FACTORY RESET FAIL - AJAX FAIL");
                },
            });
        });

        $("#editor_button_restorePoint").click(function()
        {
            //Create a jSon object and insert info

            var message = {};
            message.operation = "restore";

            // Submit via ajax

            $.ajax({
                type: "POST",
                cache: false,
                url: publicURL + '/admin/editor/postJsonFileOperation',
                data: message,
                dataType: "json",
                success: function(data)
                {
                    if (data['status'] == 'success')
                    {
                        console.log("RESTORE SUCCESS");
                        location.reload();
                    }
                    else
                    {
                        console.log("RESTORE FAIL");
                    }
                },
                error: function(data)
                {
                    console.log("RESTORE FAIL - AJAX FAIL");
                },
            });
        });

        // Build Objects

        for (x in meta)
        {
            var obj = meta[x];

            var func = getObjectFunctionPointer(obj.type, FUNCTION_TYPES.ROLLOUT);

            if (null != func)
            {
               func(obj, parentID);
               $(parentID).append("<br><hr style='height:3px;border:none; \
                                   color:#333;background-color:#333;'><br>");
            }
        }

        // Style Objects

        $(".editor_title").css({"font-size":"150%","color":"red"});
        $(".editor_info").css({"color":"green"});
        $(".editor_type").css({"display":"none"});
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Roll-In - Reads the contents of the passed in handle and parses out the TUXEDOCAT objects.
     * Returns a JSON string which represents a buildable JSON file.
    */

    function rollIn(parentID)
    {
        var fullContent = $(parentID).html();
        var partContent = "";
        var jsonStr = "";
        var objType = "";
        var indexS = 0;
        var indexE = 0;

        jsonStr += "{\n";

        while (true)
        {
            // Find the next object start index by looking for the defined key.

            if ((-1 == (indexS = fullContent.indexOf(TUXEDOCAT_EDITOR_OBJ_KEY_START, indexE))) ||
                (-1 == (indexS = fullContent.indexOf(">", indexS + 1))) ||
                (-1 == (indexE = fullContent.indexOf("<", indexS + 1))))
            {
                console.log("TUXCAT - Critical Parse Error - RollIn");
                break;
            }

            // Read the object type from the span content.

            objType = (fullContent.substring(indexS + 1, indexE)).trim();

            indexS = indexE;
            indexE = fullContent.indexOf(TUXEDOCAT_EDITOR_OBJ_KEY_START, indexE + 1);

            // If indexE is -1, we have reached the last object

            if (-1 == indexE)
            {
                partContent = fullContent.substring(indexS);
            }
            else
            {
                partContent = fullContent.substring(indexS, indexE);
            }

            var func = getObjectFunctionPointer(objType, FUNCTION_TYPES.ROLLIN);

            if (null != func)
            {
                jsonStr += func(partContent);
            }

            // If another object is following, add a comma, else, we are done.

            if (-1 != indexE)
            {
                jsonStr += ",\n\n";
            }
            else
            {
                break;
            }
        }

        jsonStr += "}";

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
        * Builds a specific object identified by objKey. This is an object inside the meta json. If
        * the object isn't found, does nothing.
    */

    this.Build_Object = function (objKey, parentID)
    {
        // Search for object

        var obj = meta[objKey];

        if (null == obj)
        {
            console.log("Invalid Object Key: " + objKey);
            return;
        }

        var func = getObjectFunctionPointer(obj.type, FUNCTION_TYPES.BUILD);

        if (null != func)
        {
            func(obj, parentID);
        }
    }

    // --------------------------------------------------------------------------------------------

    /*
     * This function exists to avoid redundant copies of the switch statement. Each object has
     * has a collection of functions which have been registered in an array. The function type
     * represents which of those functions the user wants. The list is as follows:
     *
     *  0  :  Build       - Builds html/js code from JSON input
     *  1  :  Roll-Out    - Builds editable objects for user to edit JSON file
     *  2  :  Roll-In     - Reads html content and creates a JSON string
    */

    function getObjectFunctionPointer(objTypeStr, functionType)
    {
        var functionPtr = null;

        switch(objTypeStr)
        {
            default:
                console.log("Object " + objTypeStr + " not supported.");
                break;

            case "layout":
                functionPtr = TUXCAT_LAYOUT_FUNCTIONS[functionType];
                break;

            case "text":
                functionPtr = TUXCAT_TEXT_FUNCTIONS[functionType];
                break;

            case "ftext":
                functionPtr = TUXCAT_FTEXT_FUNCTIONS[functionType];
                break;

            case "cards":
                functionPtr = TUXCAT_CARDS_FUNCTIONS[functionType];
                break;

            case "videos":
                functionPtr = TUXCAT_VIDEOS_FUNCTIONS[functionType];
                break;

            case "slider":
                functionPtr = TUXCAT_SLIDER_FUNCTIONS[functionType];
                break;

            case "carousel":
                functionPtr = TUXCAT_CAROUSEL_FUNCTIONS[functionType];
                break;

            case "backgroundImage":
                functionPtr = TUXCAT_BACKGROUNDIMAGE_FUNCTIONS[functionType];
                break;
        }

        return functionPtr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Escapes the various non-friendly symbols within a string in order to keep compatibility
     * with JSON format. Note - 'g' stands for global replacement, else, only the first instance
     * is replaced.
    */

    function escapeSpecialCharacters(str)
    {
        str = str.replace(/\\"/g, '"');
        str = str.replace(/"/g, '\\"');

        str = str.replace(/\\'/g, "'");
        str = str.replace(/'/g, "\\'");

        return str;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Layout - Blueprint for the layout within a given element, normally the entire page.
     * The build function is kept public along with the basic build function to allow users
     * to use either method for constructing a web page.
    */

    var TUXCAT_LAYOUT_FUNCTIONS =
    [
        null,
        rollOut_layout,
        rollIn_layout
    ];

    this.Build_Layout = function (layoutKey)
    {
        // Search for object

        var layout = meta[layoutKey];

        if (null == layout)
        {
            console.log("Invalid Layout Key");
            return;
        }
        else if (layout.type != "layout")
        {
            console.log("Invalid Object Type");
            return;
        }

        // Create spans for each object and build

        var parentID = document.getElementById(layout.parentID);
        $(parentID).append('<span id="' + layout.title + '"></span>');
        parentID = document.getElementById(layout.title);

        for (var i = 0; i < layout.value.length; i++)
        {
            this.Build_Object(layout.value[i], parentID);
        }
    }

    function rollOut_layout(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';
        htmlStr += '<p class="editor_parentID" style="display:none;">' +
                    metaObj.parentID + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Element ID</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var elementID = (i < metaObj.value.length) ? metaObj.value[i] : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + elementID + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_layout(str)
    {
        var objType = "layout";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract ParentID

        if ((-1 == (indexS = str.indexOf("editor_parentID", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objParentID = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            var objVal = (str.substring(indexS + 1, indexE)).trim();

            if ("" != objVal)
            {
                value.push(objVal);
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"parentID": ' + '"' + objParentID + '", \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += '"' + value[i] + '"';

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Texts - Paragraphs with no special formatting.
    */

    var TUXCAT_TEXT_FUNCTIONS =
    [
        null,
        rollOut_text,
        rollIn_text
    ];

    function rollOut_text(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';
        htmlStr += '<p class="editor_value" contenteditable="true">' + metaObj.value + '</p>';

        $(parentID).append(htmlStr);
    }

    function rollIn_text(str)
    {
        var objType = "text";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("<", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract Value

        if ((-1 == (indexS = str.indexOf("editor_value", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objValue = (str.substring(indexS + 1, indexE)).trim();

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"value": ' + '"' + objValue + '" \n';

        jsonStr += '}\n';

        return jsonStr;
    }

   // --------------------------------------------------------------------------------------------

    /*
     * Formated Texts - Paragraphs with additional css and style information.
    */

    var TUXCAT_FTEXT_FUNCTIONS =
    [
        build_ftext,
        rollOut_ftext,
        rollIn_ftext
    ];

    function build_ftext(metaObj, parentID)
    {
        var htmlStr = "";
        var id = "";
        var dateStr = Date.now().toString();

        for (var i = 0; i < metaObj.value.length; i++)
        {
            id = "ftext_" + i.toString() + dateStr;

            htmlStr +=
                '<p class="' + metaObj.value[i].class +
                '" style="' + metaObj.value[i].style +
                '" id="' + id +
                '">' + metaObj.value[i].text +
                '</p>';
        }

        $(parentID).append(htmlStr);
    }

    function rollOut_ftext(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Class</th> \
                        <th>Style</th> \
                        <th>Text</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var _class = (i < metaObj.value.length) ? metaObj.value[i].class : "";
            var _style = (i < metaObj.value.length) ? metaObj.value[i].style : "";
            var text = (i < metaObj.value.length) ? metaObj.value[i].text : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + _class + '</td> \
                            <td contenteditable="true">' + _style + '</td> \
                            <td contenteditable="true">' + text + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_ftext(str)
    {
        var objType = "ftext";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            var valStyle = "";
            var valClass = "";
            var valText = "";

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valClass = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valStyle = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valText = (str.substring(indexS + 1, indexE)).trim();
            valText = escapeSpecialCharacters(valText);

            if ("" != valText)
            {
                value.push('{\n"class":"' + valClass +
                            '",\n"style":"' + valStyle +
                            '",\n"text":"' + valText +
                            '"\n}');
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += value[i];

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Background Image - Stretches a picture to match another objects length and width. Places
     * at z-index according to meta.
    */

    var TUXCAT_BACKGROUNDIMAGE_FUNCTIONS =
    [
        build_backgroundImage,
        rollOut_backgroundImage,
        rollIn_backgroundImage
    ];

    function build_backgroundImage(metaObj, parentID)
    {
        var htmlID = document.getElementById("html");
        var id = "backgroundImage_" + Date.now();
        var htmlStr = "";

        htmlStr +=
            '<img src="" id="' + id +
            '" style="position: fixed; visibility: hidden; z-index:' + metaObj.zindex +
            '">';

        $(parentID).prepend(htmlStr);

        // Because much of the content is being loaded via JS, the resize needs to take a few
        // times while the page is getting up and loaded. Do this resize on a interval basis
        // for N seconds. Then, only do resize upon page size change.

        var pageLoadResizeCount = 0;

        function resizeBackgroundImage()
        {
            var refHeight = $(htmlID).height();
            var refWidth = $(htmlID).width();
            var aspectRatio = refWidth / refHeight;

            // Get the closest matching image based off aspect ratio

            var closestDiff = 999;
            var closestIndex = 0;
            for (var i = 0; i < metaObj.value.length; i++)
            {
                var absDiff =  Math.abs(aspectRatio - metaObj.value[i].ratio);

                if (closestDiff > absDiff)
                {
                    closestDiff = absDiff;
                    closestIndex = i;
                }
            }

            // Set background to closest aspect ratio image and resize to match container

            $('#' + id).attr("src", publicURL + metaObj.value[closestIndex].image);
            $('#' + id).height(refHeight);
            $('#' + id).width(refWidth);

            // Set image visible after small delay, measured in reset counts. This is to
            // avoid seeing a transition of different image aspect ratios as the page loads.

            if (pageLoadResizeCount == metaObj.visibleAfterCount)
            {
                $('#' + id).css("visibility", "visible");
            }

            // Continue to resize the image while the rest of the page is constructed and
            // continues to change size.

            if (metaObj.initRefreshCount > pageLoadResizeCount)
            {
                pageLoadResizeCount++;
                setTimeout(resizeBackgroundImage, metaObj.initRefreshInterval);
            }
        }

        $(window).resize(function()
        {
            resizeBackgroundImage();
        });

        resizeBackgroundImage();
    }

    function rollOut_backgroundImage(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Init Refresh Ct.</th> \
                        <th>Init Refresh Int. (MS)</th> \
                        <th>Visible After Ct.</th>  \
                        <th>Z-Index</th> \
                    </tr></thead><tbody> \
                        <td class="editor_initRefreshCount" contenteditable="true">' +
                        metaObj.initRefreshCount + '</td> \
                        <td class="editor_initRefreshInterval" contenteditable="true">' +
                        metaObj.initRefreshInterval + '</td> \
                        <td class="editor_visibleAfterCount" contenteditable="true">' +
                        metaObj.visibleAfterCount + '</td> \
                        <td class="editor_zindex" contenteditable="true">' +
                        metaObj.zindex + '</td> \
                    </tr></tbody></table>';

        htmlStr += '<br><br>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Aspect Ratio W:H</th> \
                        <th>Image</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var aspect = (i < metaObj.value.length) ? metaObj.value[i].ratio : "";
            var image = (i < metaObj.value.length) ? metaObj.value[i].image : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + aspect + '</td> \
                            <td contenteditable="true">' + image + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_backgroundImage(str)
    {
        var objType = "backgroundImage";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract Init Refresh Count

        if ((-1 == (indexS = str.indexOf("editor_initRefreshCount", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objInitRefreshCount = (str.substring(indexS + 1, indexE)).trim();

        // Extract Init Refresh Interval

        if ((-1 == (indexS = str.indexOf("editor_initRefreshInterval", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objInitRefreshInterval = (str.substring(indexS + 1, indexE)).trim();

        // Extract Visible After Count

        if ((-1 == (indexS = str.indexOf("editor_visibleAfterCount", indexE))) ||
        (-1 == (indexS = str.indexOf(">", indexS))) ||
        (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objVisibleAfterCount = (str.substring(indexS + 1, indexE)).trim();

        // Extract Z-Index

        if ((-1 == (indexS = str.indexOf("editor_zindex", indexE))) ||
        (-1 == (indexS = str.indexOf(">", indexS))) ||
        (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objZindex = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            var valRatio = "";
            var valImage = "";

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valRatio = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valImage = (str.substring(indexS + 1, indexE)).trim();

            if (("" != valRatio) || ("" != valImage))
            {
                value.push('{\n"ratio":' + valRatio +
                        ',\n"image":"' + valImage +
                        '"\n}');
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                    '"title": ' + '"' + objTitle + '", \n' +
                    '"info": ' + '"' + objInfo + '", \n' +
                    '"type": ' + '"' + objType + '", \n' +
                    '"zindex": ' + objZindex + ', \n' +
                    '"initRefreshCount": ' + objInitRefreshCount + ', \n' +
                    '"initRefreshInterval": ' + objInitRefreshInterval + ', \n' +
                    '"visibleAfterCount": ' + objVisibleAfterCount + ', \n' +
                    '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += value[i];

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Carousels - Pictures are placed on a rotating spindle for neat viewing experience.
    */

    var TUXCAT_CAROUSEL_FUNCTIONS =
    [
        build_carousel,
        rollOut_carousel,
        rollIn_carousel
    ];

    function build_carousel(metaObj, parentID)
    {
        var id = "carousel_" + Date.now();
        var htmlStr = "";

        htmlStr +=
            '<div class="row"><div class="' + metaObj.col +
            '"><div class="carousel" id="' + id +
            '">';

        for (var i = 0; i < metaObj.value.length; i++)
        {
            htmlStr +=
                '<a class="carousel-item" href="#!"><img src="' + publicURL + metaObj.value[i] +
                '"></a>';
        }

        htmlStr += '</div></div></div>';

        $(parentID).append(htmlStr);
        $('#' + id).carousel();

        // The carousel is initialized to behave well at initial window size. If window
        // size is adjusted, destroy and rebuild it.

        $( window ).resize(function() {
            $('#' + id).carousel('destroy');
            $('#' + id).carousel();
        });
    }

    function rollOut_carousel(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Columns</th> \
                    </tr></thead><tbody> \
                        <td class="editor_col" contenteditable="true">' + metaObj.col +
                        '</td> \
                    </tr></tbody></table>';

        htmlStr += '<br><br>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Image</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var image = (i < metaObj.value.length) ? metaObj.value[i] : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + image + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_carousel(str)
    {
        var objType = "carousel";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract Col

        if ((-1 == (indexS = str.indexOf("editor_col", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objCol = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            var objVal = (str.substring(indexS + 1, indexE)).trim();

            if ("" != objVal)
            {
                value.push(objVal);
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"col": ' + '"' + objCol + '", \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += '"' + value[i] + '"';

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Sliders - Slides containing picture and words on top. The words slide in from a direction
     * of your chosing.
    */

    var TUXCAT_SLIDER_FUNCTIONS =
    [
        build_slider,
        rollOut_slider,
        rollIn_slider
    ];

    function build_slider(metaObj, parentID)
    {
        var id = "slider_" + Date.now();
        var htmlStr = "";

        htmlStr +=
            '<div class="row"><div class="' + metaObj.col +
            '"><div class="slider" id="' + id +
            '"><ul class="slides">';

        for (var i = 0; i < metaObj.value.length; i++)
        {
            htmlStr +=
                '<li><img src="' + publicURL + metaObj.value[i].image +
                '"><div class="caption ' + metaObj.value[i].align +
                '"><h3>' + metaObj.value[i].title +
                '</h3><h5 class="light grey-text text-lighten-3">' + metaObj.value[i].info +
                '</h5> </div></li>';
        }

        htmlStr += '</ul></div></div></div>';

        $(parentID).append(htmlStr);
        $('#' + id).slider({interval:(metaObj.interval)});
    }

    function rollOut_slider(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Columns</th> \
                        <th>Interval</th> \
                    </tr></thead><tbody> \
                        <td class="editor_col" contenteditable="true">' + metaObj.col +
                        '</td> \
                        <td class="editor_interval" contenteditable="true">' + metaObj.interval +
                        '</td> \
                    </tr></tbody></table>';

        htmlStr += '<br><br>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Image</th> \
                        <th>Align</th> \
                        <th>Title</th> \
                        <th>Info</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var title = (i < metaObj.value.length) ? metaObj.value[i].title : "";
            var info = (i < metaObj.value.length) ? metaObj.value[i].info : "";
            var align = (i < metaObj.value.length) ? metaObj.value[i].align : "";
            var image = (i < metaObj.value.length) ? metaObj.value[i].image : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + image + '</td> \
                            <td contenteditable="true">' + align + '</td> \
                            <td contenteditable="true">' + title + '</td> \
                            <td contenteditable="true">' + info + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_slider(str)
    {
        var objType = "slider";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract col

        if ((-1 == (indexS = str.indexOf("editor_col", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objCol = (str.substring(indexS + 1, indexE)).trim();

        // Extract interval

        if ((-1 == (indexS = str.indexOf("editor_interval", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objInterval = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            var valTitle = "";
            var valInfo = "";
            var valAlign = "";
            var valImage = "";

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valImage = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valAlign = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valTitle = (str.substring(indexS + 1, indexE)).trim();
            valTitle = escapeSpecialCharacters(valTitle);

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valInfo = (str.substring(indexS + 1, indexE)).trim();
            valInfo = escapeSpecialCharacters(valInfo);

            if (("" != valTitle) || ("" != valInfo) || ("" != valImage))
            {
                value.push('{\n"title":"' + valTitle +
                        '",\n"info":"' + valInfo +
                        '",\n"align":"' + valAlign +
                        '",\n"image":"' + valImage +
                        '"\n}');
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"col": ' + '"' + objCol + '", \n' +
                   '"interval": ' + objInterval + ', \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += value[i];

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Videos - Box with embedded video.
    */

    var TUXCAT_VIDEOS_FUNCTIONS =
    [
        build_videos,
        rollOut_videos,
        rollIn_videos
    ];

    function build_videos(metaObj, parentID)
    {
        var htmlStr = "";

        htmlStr += '<div class="row">';

        for (var i = 0; i < metaObj.value.length; i++)
        {
            htmlStr +=
                '<div class="' + metaObj.col +
                '" style="padding-top: 15px;"> <div class="video-container"> \
                <iframe width="640" height="480" src="' + metaObj.value[i].url +
                '" frameborder="0" allowfullscreen></iframe></div></div>';
        }

        htmlStr += '</div>';

        $(parentID).append(htmlStr);
    }

    function rollOut_videos(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Columns</th> \
                    </tr></thead><tbody> \
                        <td class="editor_col" contenteditable="true">' + metaObj.col +
                        '</td> \
                    </tr></tbody></table>';

        htmlStr += '<br><br>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>URL</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var url = (i < metaObj.value.length) ? metaObj.value[i].url : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + url + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_videos(str)
    {
        var objType = "videos";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract col

        if ((-1 == (indexS = str.indexOf("editor_col", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objCol = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            var valUrl = "";

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valUrl = (str.substring(indexS + 1, indexE)).trim();

            if ("" != valUrl)
            {
                value.push('{\n"url":"' + valUrl + '"\n}');
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"col": ' + '"' + objCol + '", \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += value[i];

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }

    // --------------------------------------------------------------------------------------------

    /*
     * Cards - Box with image, title, and additional info. The user clicks on the cards to
     * slide the image out of the way to reveal text.
    */

    var TUXCAT_CARDS_FUNCTIONS =
    [
        build_cards,
        rollOut_cards,
        rollIn_cards
    ];

    function build_cards(metaObj, parentID)
    {
        var htmlStr = "";

        htmlStr += '<div class="row">';

        for (var i = 0; i < metaObj.value.length; i++)
        {
            htmlStr +=
                '<div class="' + metaObj.col +
                '"><div class="card"><div class="card-image waves-effect waves-block waves-light"> \
                <img class="activator" src="' + publicURL + metaObj.value[i].image +
                '"></div><div class="card-content"> \
                <span class="card-title activator grey-text text-darken-4">' + metaObj.value[i].title +
                '<i class="material-icons right">more_vert</i></span> \
                <p><a class="activator" href="#!">More info...</a></p></div><div class="card-reveal"> \
                <span class="card-title grey-text text-darken-4">' + metaObj.value[i].title +
                '<i class="material-icons right">close</i></span><p>' + metaObj.value[i].info +
                '</p></div></div></div>';
        }

        htmlStr += '</div>';

        $(parentID).append(htmlStr);
    }

    function rollOut_cards(metaObj, parentID)
    {
        var htmlStr = "";

        // Add object start indication flag

        htmlStr += '<span class="' + TUXEDOCAT_EDITOR_OBJ_KEY_START +
                   '" style="display:none;">' + metaObj.type + '</span>';

        htmlStr += '<p class="editor_title">' + metaObj.title + '</p>';
        htmlStr += '<p class="editor_info">' + metaObj.info + '</p>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Columns</th> \
                    </tr></thead><tbody> \
                        <td class="editor_col" contenteditable="true">' + metaObj.col +
                        '</td> \
                    </tr></tbody></table>';

        htmlStr += '<br><br>';

        htmlStr += '<table class="bordered"> \
                    <thead><tr> \
                        <th>Image</th> \
                        <th>Title</th> \
                        <th>Info</th> \
                    </tr></thead><tbody>';

        for (var i = 0; i < metaObj.value.length + EXTRA_ROWS; i++)
        {
            var title = (i < metaObj.value.length) ? metaObj.value[i].title : "";
            var info = (i < metaObj.value.length) ? metaObj.value[i].info : "";
            var image = (i < metaObj.value.length) ? metaObj.value[i].image : "";

            htmlStr += '<tr> \
                            <td contenteditable="true">' + image + '</td> \
                            <td contenteditable="true">' + title + '</td> \
                            <td contenteditable="true">' + info + '</td> \
                        </tr>';
        }

        htmlStr += '</tbody></table>';

        $(parentID).append(htmlStr);
    }

    function rollIn_cards(str)
    {
        var objType = "cards";
        var jsonStr = "";
        var indexS = 0;
        var indexE = 0;

        // Extract Title

        if ((-1 == (indexS = str.indexOf("editor_title", indexS))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objTitle = (str.substring(indexS + 1, indexE)).trim();

        // Extract info

        if ((-1 == (indexS = str.indexOf("editor_info", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</p>", indexS))))
        {
            return "";
        }
        var objInfo = (str.substring(indexS + 1, indexE)).trim();

        // Extract col

        if ((-1 == (indexS = str.indexOf("editor_col", indexE))) ||
            (-1 == (indexS = str.indexOf(">", indexS))) ||
            (-1 == (indexE = str.indexOf("</td>", indexS))))
        {
            return "";
        }
        var objCol = (str.substring(indexS + 1, indexE)).trim();

        // Extract Values

        var value = [];

        while (true)
        {
            var valTitle = "";
            var valInfo = "";
            var valImage = "";

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valImage = (str.substring(indexS + 1, indexE)).trim();

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valTitle = (str.substring(indexS + 1, indexE)).trim();
            valTitle = escapeSpecialCharacters(valTitle);

            if ((-1 == (indexS = str.indexOf("<td", indexE))) ||
                (-1 == (indexS = str.indexOf(">", indexS))) ||
                (-1 == (indexE = str.indexOf("</td>", indexS))))
            {
                break;
            }
            valInfo = (str.substring(indexS + 1, indexE)).trim();
            valInfo = escapeSpecialCharacters(valInfo);

            if (("" != valTitle) || ("" != valInfo) || ("" != valImage))
            {
                value.push('{\n"title":"' + valTitle +
                        '",\n"info":"' + valInfo +
                        '",\n"image":"' + valImage +
                        '"\n}');
            }
        }

        // Build JSON

        jsonStr += '"' + objTitle + '": { \n' +
                   '"title": ' + '"' + objTitle + '", \n' +
                   '"info": ' + '"' + objInfo + '", \n' +
                   '"type": ' + '"' + objType + '", \n' +
                   '"col": ' + '"' + objCol + '", \n' +
                   '"value": [ \n';

        for (var i = 0; i < value.length; i++)
        {
            jsonStr += value[i];

            if (value.length > (i + 1))
            {
                jsonStr += ', \n';
            }
            else
            {
                jsonStr += ' \n';
            }
        }

        jsonStr += ']\n}\n';

        return jsonStr;
    }
}

