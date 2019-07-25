/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  October 12, 2015
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.3
** Version Date: Nov. 21, 2016
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
Stand Alone, tifyBox 3D Module. Requires canvas parent ID to be passed in. The parent canvas MUST
be a max w/h of 1400px.

Dependency: jQuery

Translation: Shift+MouseDown+MouseMove || Two Finger Swipe
 Translation is very accurate b/c the object is analyzed in model space. Every time the user
 re-adjusts the zoom factor, new values are found which determine the movement ratio from screen
 movement to model view movement.
Rotation: MouseDown+MouseMove || One Finger Swipe
 Also uses the ratio factor which translate uses to get an appropriate rotation amount. This
 effectively "slows down" the rotation as the user zooms in closer to tifyBox.
Selection: Click || Touch
 Selection is done via my own version of picking. We are fragment picking, not vertex triangle
 picking. The coords of click are given to the fragment shader. Intersection with these coords is
 tested for, and the results are written to a separate frame buffer texture. The texture is then
 analyzed to determine the u,v texture coordinates which were encoded into a RGBA value in the FS,
 which is then used to represent the face, X, and Y.
Touch Events
 This was a challenge by itself due to random anomalies in sensitive screens. When you put your
 finger down you are literally registering several touches, b/c of blood in the finger I denoted.
 This has been taken into account and the touch events seem to work wonderfully!
Transform to a given F,X,Y
 The transform is calculated out to find the needed rotation angle and distance. The work is then
 subdivided up to occur within several draw events.

****************************************************************************************************
CHANGE LOG *****************************************************************************************
****************************************************************************************************
Oct. 12, 2015 - Jared Fowler
Changed the order of the faces to match tify. Updated commentaries in code. Deleted interval to take
exposure sample. It is now controlled by function call, which makes the overall experience a tad
better! Changed the fragment texture to have a width and height of 1400. This will require that the
canvas be limited to these dimensions!

Oct. 13, 2015 - Jared Fowler
Modified code to not submit user clicks to new window. Instead, the class will simply update the
selection info and rely on the parent container to check for updated selections. A mutex check was
added for click events.

Nov. 21, 2016 - Jared Fowler
Removed several checks which had to do with unused alpha. Tifybox has a new architecture where every
square should be filled and have a valid URL.

Jan. 31, 2017 - Jared Fowler
Tifybox has become a pure YouTube interface. All blocks are not guaranteed to be filled, hence,
going back to older architecture.

****************************************************************************************************
***************************************************************************************************/

//TWGL - Modified to allow string input for shader program compilation.
!function(a,b){"function"==typeof define&&define.amd?define([],b):a.twgl=b()}(this,function(){var a,b,c;return function(d){function e(a,b){return u.call(a,b)}function f(a,b){var c,d,e,f,g,h,i,j,k,l,m,n=b&&b.split("/"),o=s.map,p=o&&o["*"]||{};if(a&&"."===a.charAt(0))if(b){for(a=a.split("/"),g=a.length-1,s.nodeIdCompat&&w.test(a[g])&&(a[g]=a[g].replace(w,"")),a=n.slice(0,n.length-1).concat(a),k=0;k<a.length;k+=1)if(m=a[k],"."===m)a.splice(k,1),k-=1;else if(".."===m){if(1===k&&(".."===a[2]||".."===a[0]))break;k>0&&(a.splice(k-1,2),k-=2)}a=a.join("/")}else 0===a.indexOf("./")&&(a=a.substring(2));if((n||p)&&o){for(c=a.split("/"),k=c.length;k>0;k-=1){if(d=c.slice(0,k).join("/"),n)for(l=n.length;l>0;l-=1)if(e=o[n.slice(0,l).join("/")],e&&(e=e[d])){f=e,h=k;break}if(f)break;!i&&p&&p[d]&&(i=p[d],j=k)}!f&&i&&(f=i,h=j),f&&(c.splice(0,h,f),a=c.join("/"))}return a}function g(a,b){return function(){var c=v.call(arguments,0);return"string"!=typeof c[0]&&1===c.length&&c.push(null),n.apply(d,c.concat([a,b]))}}function h(a){return function(b){return f(b,a)}}function i(a){return function(b){q[a]=b}}function j(a){if(e(r,a)){var b=r[a];delete r[a],t[a]=!0,m.apply(d,b)}if(!e(q,a)&&!e(t,a))throw new Error("No "+a);return q[a]}function k(a){var b,c=a?a.indexOf("!"):-1;return c>-1&&(b=a.substring(0,c),a=a.substring(c+1,a.length)),[b,a]}function l(a){return function(){return s&&s.config&&s.config[a]||{}}}var m,n,o,p,q={},r={},s={},t={},u=Object.prototype.hasOwnProperty,v=[].slice,w=/\.js$/;o=function(a,b){var c,d=k(a),e=d[0];return a=d[1],e&&(e=f(e,b),c=j(e)),e?a=c&&c.normalize?c.normalize(a,h(b)):f(a,b):(a=f(a,b),d=k(a),e=d[0],a=d[1],e&&(c=j(e))),{f:e?e+"!"+a:a,n:a,pr:e,p:c}},p={require:function(a){return g(a)},exports:function(a){var b=q[a];return"undefined"!=typeof b?b:q[a]={}},module:function(a){return{id:a,uri:"",exports:q[a],config:l(a)}}},m=function(a,b,c,f){var h,k,l,m,n,s,u=[],v=typeof c;if(f=f||a,"undefined"===v||"function"===v){for(b=!b.length&&c.length?["require","exports","module"]:b,n=0;n<b.length;n+=1)if(m=o(b[n],f),k=m.f,"require"===k)u[n]=p.require(a);else if("exports"===k)u[n]=p.exports(a),s=!0;else if("module"===k)h=u[n]=p.module(a);else if(e(q,k)||e(r,k)||e(t,k))u[n]=j(k);else{if(!m.p)throw new Error(a+" missing "+k);m.p.load(m.n,g(f,!0),i(k),{}),u[n]=q[k]}l=c?c.apply(q[a],u):void 0,a&&(h&&h.exports!==d&&h.exports!==q[a]?q[a]=h.exports:l===d&&s||(q[a]=l))}else a&&(q[a]=c)},a=b=n=function(a,b,c,e,f){if("string"==typeof a)return p[a]?p[a](b):j(o(a,b).f);if(!a.splice){if(s=a,s.deps&&n(s.deps,s.callback),!b)return;b.splice?(a=b,b=c,c=null):a=d}return b=b||function(){},"function"==typeof c&&(c=e,e=f),e?m(d,a,b,c):setTimeout(function(){m(d,a,b,c)},4),n},n.config=function(a){return n(a)},a._defined=q,c=function(a,b,c){if("string"!=typeof a)throw new Error("See almond README: incorrect module build, no module name");b.splice||(c=b,b=[]),e(q,a)||e(r,a)||(r[a]=[a,b,c])},c.amd={jQuery:!0}}(),c("node_modules/almond/almond.js",function(){}),c("twgl/twgl",[],function(){function a(a){ea=new Uint8Array([255*a[0],255*a[1],255*a[2],255*a[3]])}function b(a){da=a}function c(a,b){for(var c=["webgl","experimental-webgl"],d=null,e=0;e<c.length;++e){try{d=a.getContext(c[e],b)}catch(f){}if(d)break}return d}function d(a,b){var d=c(a,b);return d}function e(a){return a.split("\n").map(function(a,b){return b+1+": "+a}).join("\n")}function f(a,b,c,d){var f=d||ca,g=a.createShader(c);a.shaderSource(g,b),a.compileShader(g);var h=a.getShaderParameter(g,a.COMPILE_STATUS);if(!h){var i=a.getShaderInfoLog(g);return f(e(b)+"\n*** Error compiling shader: "+i),a.deleteShader(g),null}return g}function g(a,b,c,d,e){var f=e||ca,g=a.createProgram();b.forEach(function(b){a.attachShader(g,b)}),c&&c.forEach(function(b,c){a.bindAttribLocation(g,d?d[c]:c,b)}),a.linkProgram(g);var h=a.getProgramParameter(g,a.LINK_STATUS);if(!h){var i=a.getProgramInfoLog(g);return f("Error in program linking:"+i),a.deleteProgram(g),null}return g}function h(a,b,c,d){var e,g="",h=document.getElementById(b);if(!h)throw"*** Error: unknown script element"+b;if(g=h.text,!c)if("x-shader/x-vertex"===h.type)e=a.VERTEX_SHADER;else if("x-shader/x-fragment"===h.type)e=a.FRAGMENT_SHADER;else if(e!==a.VERTEX_SHADER&&e!==a.FRAGMENT_SHADER)throw"*** Error: unknown shader type";return f(a,g,c?c:e,d)}function i(a,b,c,d,e){for(var f=[],i=0;i<b.length;++i){var j=h(a,b[i],a[Ha[i]],e);if(!j)return null;f.push(j)}return g(a,f,c,d,e)}function j(a,b,c,d,e){for(var h=[],i=0;i<b.length;++i){var j=f(a,b[i],a[Ha[i]],e);if(!j)return null;h.push(j)}return g(a,h,c,d,e)}function k(a,b){return b===a.SAMPLER_2D?a.TEXTURE_2D:b===a.SAMPLER_CUBE?a.TEXTURE_CUBE_MAP:void 0}function l(a,b){function c(b,c){var e=a.getUniformLocation(b,c.name),f=c.type,g=c.size>1&&"[0]"===c.name.substr(-3);if(f===a.FLOAT&&g)return function(b){a.uniform1fv(e,b)};if(f===a.FLOAT)return function(b){a.uniform1f(e,b)};if(f===a.FLOAT_VEC2)return function(b){a.uniform2fv(e,b)};if(f===a.FLOAT_VEC3)return function(b){a.uniform3fv(e,b)};if(f===a.FLOAT_VEC4)return function(b){a.uniform4fv(e,b)};if(f===a.INT&&g)return function(b){a.uniform1iv(e,b)};if(f===a.INT)return function(b){a.uniform1i(e,b)};if(f===a.INT_VEC2)return function(b){a.uniform2iv(e,b)};if(f===a.INT_VEC3)return function(b){a.uniform3iv(e,b)};if(f===a.INT_VEC4)return function(b){a.uniform4iv(e,b)};if(f===a.BOOL&&g)return function(b){a.uniform1iv(e,b)};if(f===a.BOOL)return function(b){a.uniform1i(e,b)};if(f===a.BOOL_VEC2)return function(b){a.uniform2iv(e,b)};if(f===a.BOOL_VEC3)return function(b){a.uniform3iv(e,b)};if(f===a.BOOL_VEC4)return function(b){a.uniform4iv(e,b)};if(f===a.FLOAT_MAT2)return function(b){a.uniformMatrix2fv(e,!1,b)};if(f===a.FLOAT_MAT3)return function(b){a.uniformMatrix3fv(e,!1,b)};if(f===a.FLOAT_MAT4)return function(b){a.uniformMatrix4fv(e,!1,b)};if((f===a.SAMPLER_2D||f===a.SAMPLER_CUBE)&&g){for(var h=[],i=0;i<c.size;++i)h.push(d++);return function(b,c){return function(d){a.uniform1iv(e,c),d.forEach(function(d,e){a.activeTexture(a.TEXTURE0+c[e]),a.bindTexture(b,d)})}}(k(a,f),h)}if(f===a.SAMPLER_2D||f===a.SAMPLER_CUBE)return function(b,c){return function(d){a.uniform1i(e,c),a.activeTexture(a.TEXTURE0+c),a.bindTexture(b,d)}}(k(a,f),d++);throw"unknown type: 0x"+f.toString(16)}for(var d=0,e={},f=a.getProgramParameter(b,a.ACTIVE_UNIFORMS),g=0;f>g;++g){var h=a.getActiveUniform(b,g);if(!h)break;var i=h.name;"[0]"===i.substr(-3)&&(i=i.substr(0,i.length-3));var j=c(b,h);e[i]=j}return e}function m(a,b){a=a.uniformSetters||a;for(var c=arguments.length,d=1;c>d;++d){var e=arguments[d];if(Array.isArray(e))for(var f=e.length,g=0;f>g;++g)m(a,e[g]);else for(var h in e){var i=a[h];i&&i(e[h])}}}function n(a,b){function c(b){return function(c){a.bindBuffer(a.ARRAY_BUFFER,c.buffer),a.enableVertexAttribArray(b),a.vertexAttribPointer(b,c.numComponents||c.size,c.type||a.FLOAT,c.normalize||!1,c.stride||0,c.offset||0)}}for(var d={},e=a.getProgramParameter(b,a.ACTIVE_ATTRIBUTES),f=0;e>f;++f){var g=a.getActiveAttrib(b,f);if(!g)break;var h=a.getAttribLocation(b,g.name);d[g.name]=c(h)}return d}function o(a,b){for(var c in b){var d=a[c];d&&d(b[c])}}function p(a,b,c){o(b.attribSetters||b,c.attribs),c.indices&&a.bindBuffer(a.ELEMENT_ARRAY_BUFFER,c.indices)}function q(a,b,c,d,e){/*b=b.map(function(a){var b=document.getElementById(a);return b?b.text:a});*/var f=j(a,b,c,d,e);if(!f)return null;var g=l(a,f),h=n(a,f);return{program:f,uniformSetters:g,attribSetters:h}}function r(a,b){b=b||1,b=Math.max(1,b);var c=a.clientWidth*b|0,d=a.clientHeight*b|0;return a.width!==c||a.height!==d?(a.width=c,a.height=d,!0):!1}function s(a,b,c,d){if(b instanceof WebGLBuffer)return b;c=c||a.ARRAY_BUFFER;var e=a.createBuffer();return a.bindBuffer(c,e),a.bufferData(c,b,d||a.STATIC_DRAW),e}function t(a){return"indices"===a}function u(a){if(a instanceof Int8Array)return ga;if(a instanceof Uint8Array)return ha;if(a instanceof Int16Array)return ia;if(a instanceof Uint16Array)return ja;if(a instanceof Int32Array)return ka;if(a instanceof Uint32Array)return la;if(a instanceof Float32Array)return ma;throw"unsupported typed array type"}function v(a,b){switch(b){case a.BYTE:return Int8Array;case a.UNSIGNED_BYTE:return Uint8Array;case a.SHORT:return Int16Array;case a.UNSIGNED_SHORT:return Uint16Array;case a.INT:return Int32Array;case a.UNSIGNED_INT:return Uint32Array;case a.FLOAT:return Float32Array;default:throw"unknown gl type"}}function w(a){return a instanceof Int8Array?!0:a instanceof Uint8Array?!0:!1}function x(a){return a&&a.buffer&&a.buffer instanceof ArrayBuffer}function y(a,b){var c;if(c=a.indexOf("coord")>=0?2:a.indexOf("color")>=0?4:3,b%c>0)throw"can not guess numComponents. You should specify it.";return c}function z(a,b){if(x(a))return a;if(x(a.data))return a.data;Array.isArray(a)&&(a={data:a});var c=a.type;return c||(c="indices"===b?Uint16Array:Float32Array),new c(a.data)}function A(a,b){var c={};return Object.keys(b).forEach(function(d){if(!t(d)){var e=b[d],f=e.attrib||e.name||e.attribName||da+d,g=z(e,d);c[f]={buffer:s(a,g,void 0,e.drawType),numComponents:e.numComponents||e.size||y(d),type:u(g),normalize:void 0!==e.normalize?e.normalize:w(g),stride:e.stride||0,offset:e.offset||0}}}),c}function B(a,b){var c={attribs:A(a,b)},d=b.indices;return d?(d=z(d,"indices"),c.indices=s(a,d,a.ELEMENT_ARRAY_BUFFER),c.numElements=d.length,c.elementType=d instanceof Uint32Array?a.UNSIGNED_INT:a.UNSIGNED_SHORT):c.numElements=Ia(b),c}function C(a,b){var c={};return Object.keys(b).forEach(function(d){var e="indices"===d?a.ELEMENT_ARRAY_BUFFER:a.ARRAY_BUFFER,f=z(b[d],d);c[d]=s(a,f,e)}),c}function D(a,b,c,d,e){var f=c.indices,g=void 0===d?c.numElements:d;e=void 0===e?0:e,f?a.drawElements(b,g,void 0===c.elementType?a.UNSIGNED_SHORT:c.elementType,e):a.drawArrays(b,e,g)}function E(a,b){var c=null,d=null;b.forEach(function(b){if(b.active!==!1){var e=b.programInfo,f=b.bufferInfo,g=!1;e!==c&&(c=e,a.useProgram(e.program),g=!0),(g||f!==d)&&(d=f,p(a,e,f)),m(e,b.uniforms),D(a,b.type||a.TRIANGLES,f,b.count,b.offset)}})}function F(a,b){void 0!==b.colorspaceConversion&&(Ja.colorSpaceConversion=a.getParameter(a.UNPACK_COLORSPACE_CONVERSION_WEBGL)),void 0!==b.premultiplyAlpha&&(Ja.premultiplyAlpha=a.getParameter(a.UNPACK_PREMULTIPLY_ALPHA_WEBGL)),void 0!==b.flipY&&(Ja.flipY=a.getParameter(a.UNPACK_FLIP_Y_WEBGL))}function G(a,b){void 0!==b.colorspaceConversion&&a.pixelStorei(a.UNPACK_COLORSPACE_CONVERSION_WEBGL,Ja.colorSpaceConversion),void 0!==b.premultiplyAlpha&&a.pixelStorei(a.UNPACK_PREMULTIPLY_ALPHA_WEBGL,Ja.premultiplyAlpha),void 0!==b.flipY&&a.pixelStorei(a.UNPACK_FLIP_Y_WEBGL,Ja.flipY)}function H(a,b,c){var d=c.target||a.TEXTURE_2D;a.bindTexture(d,b),c.min&&a.texParameteri(d,a.TEXTURE_MIN_FILTER,c.min),c.mag&&a.texParameteri(d,a.TEXTURE_MAG_FILTER,c.mag),c.wrap&&(a.texParameteri(d,a.TEXTURE_WRAP_S,c.wrap),a.texParameteri(d,a.TEXTURE_WRAP_T,c.wrap)),c.wrapS&&a.texParameteri(d,a.TEXTURE_WRAP_S,c.wrapS),c.wrapT&&a.texParameteri(d,a.TEXTURE_WRAP_T,c.wrapT)}function I(a){return a=a||ea,x(a)?a:new Uint8Array([255*a[0],255*a[1],255*a[2],255*a[3]])}function J(a){return 0===(a&a-1)}function K(a,b,c,d,e){c=c||fa;var f=c.target||a.TEXTURE_2D;d=d||c.width,e=e||c.height,a.bindTexture(f,b),J(d)&&J(e)?a.generateMipmap(f):(a.texParameteri(f,a.TEXTURE_MIN_FILTER,a.LINEAR),a.texParameteri(f,a.TEXTURE_WRAP_S,a.CLAMP_TO_EDGE),a.texParameteri(f,a.TEXTURE_WRAP_T,a.CLAMP_TO_EDGE))}function L(a,b){return b=b||{},b.cubeFaceOrder||[a.TEXTURE_CUBE_MAP_POSITIVE_X,a.TEXTURE_CUBE_MAP_NEGATIVE_X,a.TEXTURE_CUBE_MAP_POSITIVE_Y,a.TEXTURE_CUBE_MAP_NEGATIVE_Y,a.TEXTURE_CUBE_MAP_POSITIVE_Z,a.TEXTURE_CUBE_MAP_NEGATIVE_Z]}function M(a,b){var c=L(a,b),d=c.map(function(a,b){return{face:a,ndx:b}});return d.sort(function(a,b){return a.face-b.face}),d}function N(a){var b={};return Object.keys(a).forEach(function(c){b[c]=a[c]}),b}function O(a,b){var c=new Image;return c.onerror=function(){var d="couldn't load image: "+a;ca(d),b(d,c)},c.onload=function(){b(null,c)},c.src=a,c}function P(a,b,c){c=c||fa;var d=c.target||a.TEXTURE_2D;if(a.bindTexture(d,b),c.color!==!1){var e=I(c.color);if(d===a.TEXTURE_CUBE_MAP)for(var f=0;6>f;++f)a.texImage2D(a.TEXTURE_CUBE_MAP_POSITIVE_X+f,0,a.RGBA,1,1,0,a.RGBA,a.UNSIGNED_BYTE,e);else a.texImage2D(d,0,a.RGBA,1,1,0,a.RGBA,a.UNSIGNED_BYTE,e)}}function Q(a,b,c,d){c=c||fa,P(a,b,c),c=N(c);var e=O(c.src,function(e,f){e?d(e,b,f):(Ka(a,b,f,c),d(null,b,f))});return e}function R(a,b,c,d){function e(e){return function(f,m){--k,f?l.push(f):m.width!==m.height?l.push("cubemap face img is not a square: "+m.src):(F(a,c),a.bindTexture(i,b),5===k?L(a).forEach(function(b){a.texImage2D(b,0,g,g,h,m)}):a.texImage2D(e,0,g,g,h,m),G(a,c),a.generateMipmap(i)),0===k&&d&&d(l.length?l:void 0,j,b)}}var f=c.src;if(6!==f.length)throw"there must be 6 urls for a cubemap";var g=c.format||a.RGBA,h=c.type||a.UNSIGNED_BYTE,i=c.target||a.TEXTURE_2D;if(i!==a.TEXTURE_CUBE_MAP)throw"target must be TEXTURE_CUBE_MAP";P(a,b,c),c=N(c);var j,k=6,l=[],m=L(a,c);j=f.map(function(a,b){return O(a,e(m[b]))})}function S(a){switch(a){case oa:case ra:return 1;case sa:return 2;case pa:return 3;case qa:return 4;default:throw"unknown type: "+a}}function T(a,b){return x(b)?u(b):a.UNSIGNED_BYTE}function U(a,b,c,d){d=d||fa;var e=d.target||a.TEXTURE_2D,f=d.width,g=d.height,h=d.format||a.RGBA,i=d.type||T(a,c),j=S(h),k=c.length/j;if(k%1)throw"length wrong size of format: "+Ga(a,h);if(f||g){if(g){if(!f&&(f=k/g,f%1))throw"can't guess width"}else if(g=k/f,g%1)throw"can't guess height"}else{var l=Math.sqrt(k/(e===a.TEXTURE_CUBE_MAP?6:1));l%1===0?(f=l,g=l):(f=k,g=1)}if(!x(c)){var m=v(a,i);c=new m(c)}if(a.pixelStorei(a.UNPACK_ALIGNMENT,d.unpackAlignment||1),F(a,d),e===a.TEXTURE_CUBE_MAP){var n=k/6*j;M(a,d).forEach(function(b){var d=n*b.ndx,e=c.subarray(d,d+n);a.texImage2D(b.face,0,h,f,g,0,h,i,e)})}else a.texImage2D(e,0,h,f,g,0,h,i,c);return G(a,d),{width:f,height:g}}function V(a,b,c){var d=c.target||a.TEXTURE_2D;a.bindTexture(d,b);var e=c.format||a.RGBA,f=c.type||a.UNSIGNED_BYTE;if(F(a,c),d===a.TEXTURE_CUBE_MAP)for(var g=0;6>g;++g)a.texImage2D(a.TEXTURE_CUBE_MAP_POSITIVE_X+g,0,e,c.width,c.height,0,e,f,null);else a.texImage2D(d,0,e,c.width,c.height,0,e,f,null)}function W(a,b,c){b=b||fa;var d=a.createTexture(),e=b.target||a.TEXTURE_2D,f=b.width||1,g=b.height||1;a.bindTexture(e,d),e===a.TEXTURE_CUBE_MAP&&(a.texParameteri(e,a.TEXTURE_WRAP_S,a.CLAMP_TO_EDGE),a.texParameteri(e,a.TEXTURE_WRAP_T,a.CLAMP_TO_EDGE));var h=b.src;if(h)if("function"==typeof h&&(h=h(a,b)),"string"==typeof h)Q(a,d,b,c);else if(x(h)||Array.isArray(h)&&("number"==typeof h[0]||Array.isArray(h[0])||x(h[0]))){var i=U(a,d,h,b);f=i.width,g=i.height}else if(Array.isArray(h)&&"string"==typeof h[0])R(a,d,b,c);else{if(!(h instanceof HTMLElement))throw"unsupported src type";Ka(a,d,h,b),f=h.width,g=h.height}else V(a,d,b);return b.auto!==!1&&K(a,d,b,f,g),H(a,d,b),d}function X(a,b,c,d,e){d=d||c.width,e=e||c.height;var f=c.target||a.TEXTURE_2D;a.bindTexture(f,b);var g,h=c.format||a.RGBA,i=c.src;if(g=i&&(x(i)||Array.isArray(i)&&"number"==typeof i[0])?c.type||T(a,i):c.type||a.UNSIGNED_BYTE,f===a.TEXTURE_CUBE_MAP)for(var j=0;6>j;++j)a.texImage2D(a.TEXTURE_CUBE_MAP_POSITIVE_X+j,0,h,d,e,0,h,g,null);else a.texImage2D(f,0,h,d,e,0,h,g,null)}function Y(a){return"string"==typeof a||Array.isArray(a)&&"string"==typeof a[0]}function Z(a,b,c){function d(){0===f&&c&&setTimeout(function(){c(g.length?g:void 0,b)},0)}function e(a){--f,a&&g.push(a),d()}var f=0,g=[],h={};return Object.keys(b).forEach(function(c){var d=b[c],g=void 0;Y(d.src)&&(g=e,++f),h[c]=W(a,d,g)}),d(),h}function $(a){return Ma[a]}function _(a){return Na[a]}function aa(a,b,c,d){var e=a.FRAMEBUFFER,f=a.createFramebuffer();a.bindFramebuffer(e,f),c=c||a.drawingBufferWidth,d=d||a.drawingBufferHeight,b=b||La;var g=0,h={framebuffer:f,attachments:[]};return b.forEach(function(b){var f=b.attachment,i=b.format,j=$(i);if(j||(j=Aa+g++),!f)if(_(i))f=a.createRenderbuffer(),a.bindRenderbuffer(a.RENDERBUFFER,f),a.renderbufferStorage(a.RENDERBUFFER,i,c,d);else{var k=N(b);k.width=c,k.height=d,k.auto=void 0===b.auto?!1:b.auto,f=W(a,k)}if(f instanceof WebGLRenderbuffer)a.framebufferRenderbuffer(e,j,a.RENDERBUFFER,f);else{if(!(f instanceof WebGLTexture))throw"unknown attachment type";a.framebufferTexture2D(e,j,b.texTarget||a.TEXTURE_2D,f,b.level||0)}h.attachments.push(f)}),h}function ba(a,b,c,d,e){d=d||a.drawingBufferWidth,e=e||a.drawingBufferHeight,c=c||La,c.forEach(function(c,f){var g=b.attachments[f],h=c.format;if(g instanceof WebGLRenderbuffer)a.bindRenderbuffer(a.RENDERBUFFER,g),a.renderbufferStorage(a.RENDERBUFFER,h,d,e);else{if(!(g instanceof WebGLTexture))throw"unknown attachment type";X(a,g,c,d,e)}})}var ca=window.console&&window.console.error?window.console.error.bind(window.console):function(){},da="",ea=new Uint8Array([128,192,255,255]),fa={},ga=5120,ha=5121,ia=5122,ja=5123,ka=5124,la=5125,ma=5126,na=6402,oa=6406,pa=6407,qa=6408,ra=6409,sa=6410,ta=32854,ua=32855,va=36194,wa=33189,xa=6401,ya=36168,za=34041,Aa=36064,Ba=36096,Ca=36128,Da=33306,Ea=33071,Fa=9729,Ga=function(){function a(a){b||(b={},Object.keys(a).forEach(function(c){"number"==typeof a[c]&&(b[a[c]]=c)}))}var b;return function(c,d){return a(),b[d]||"0x"+d.toString(16)}}(),Ha=["VERTEX_SHADER","FRAGMENT_SHADER"],Ia=function(){var a=["position","positions","a_position"];return function(b){for(var c,d=0;d<a.length&&(c=a[d],!(c in b));++d);d===a.length&&(c=Object.keys(b)[0]);var e=b[c],f=e.length||e.data.length,g=e.numComponents||y(c,f),h=f/g;if(f%g>0)throw"numComponents "+g+" not correct for length "+f;return h}}(),Ja={},Ka=function(){var a=document.createElement("canvas").getContext("2d");return function(b,c,d,e){e=e||fa;var f=e.target||b.TEXTURE_2D,g=d.width,h=d.height,i=e.format||b.RGBA,j=e.type||b.UNSIGNED_BYTE;if(F(b,e),b.bindTexture(f,c),f===b.TEXTURE_CUBE_MAP){var k,l,m=d.width,n=d.height;if(m/6===n)k=n,l=[0,0,1,0,2,0,3,0,4,0,5,0];else if(n/6===m)k=m,l=[0,0,0,1,0,2,0,3,0,4,0,5];else if(m/3===n/2)k=m/3,l=[0,0,1,0,2,0,0,1,1,1,2,1];else{if(m/2!==n/3)throw"can't figure out cube map from element: "+(d.src?d.src:d.nodeName);k=m/2,l=[0,0,1,0,0,1,1,1,0,2,1,2]}a.canvas.width=k,a.canvas.height=k,g=k,h=k,M(b,e).forEach(function(c){var e=l[2*c.ndx+0]*k,f=l[2*c.ndx+1]*k;a.drawImage(d,e,f,k,k,0,0,k,k),b.texImage2D(c.face,0,i,i,j,a.canvas)}),a.canvas.width=1,a.canvas.height=1}else b.texImage2D(f,0,i,i,j,d);G(b,e),e.auto!==!1&&K(b,c,e,g,h),H(b,c,e)}}(),La=[{format:qa,type:ha,min:Fa,wrap:Ea},{format:za}],Ma={};Ma[za]=Da,Ma[xa]=Ca,Ma[ya]=Ca,Ma[na]=Ba,Ma[wa]=Ba;var Na={};return Na[ta]=!0,Na[ua]=!0,Na[va]=!0,Na[za]=!0,Na[wa]=!0,Na[xa]=!0,Na[ya]=!0,{createAttribsFromArrays:A,createBuffersFromArrays:C,createBufferInfoFromArrays:B,createAttributeSetters:n,createProgram:g,createProgramFromScripts:i,createProgramFromSources:j,createProgramInfo:q,createUniformSetters:l,drawBufferInfo:D,drawObjectList:E,getWebGLContext:d,resizeCanvasToDisplaySize:r,setAttributes:o,setAttributePrefix:b,setBuffersAndAttributes:p,setUniforms:m,createTexture:W,setEmptyTexture:V,setTextureFromArray:U,loadTextureFromUrl:Q,setTextureFromElement:Ka,setTextureFilteringForSize:K,setTextureParameters:H,setDefaultTextureColor:a,createTextures:Z,resizeTexture:X,createFramebufferInfo:aa,resizeFramebufferInfo:ba}}),c("twgl/v3",[],function(){function a(a){q=a}function b(){return new q(3)}function c(a,b,c){return c=c||new q(3),c[0]=a[0]+b[0],c[1]=a[1]+b[1],c[2]=a[2]+b[2],c}function d(a,b,c){return c=c||new q(3),c[0]=a[0]-b[0],c[1]=a[1]-b[1],c[2]=a[2]-b[2],c}function e(a,b,c,d){return d=d||new q(3),d[0]=(1-c)*a[0]+c*b[0],d[1]=(1-c)*a[1]+c*b[1],d[2]=(1-c)*a[2]+c*b[2],d}function f(a,b,c){return c=c||new q(3),c[0]=a[0]*b,c[1]=a[1]*b,c[2]=a[2]*b,c}function g(a,b,c){return c=c||new q(3),c[0]=a[0]/b,c[1]=a[1]/b,c[2]=a[2]/b,c}function h(a,b,c){return c=c||new q(3),c[0]=a[1]*b[2]-a[2]*b[1],c[1]=a[2]*b[0]-a[0]*b[2],c[2]=a[0]*b[1]-a[1]*b[0],c}function i(a,b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]}function j(a){return Math.sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])}function k(a){return a[0]*a[0]+a[1]*a[1]+a[2]*a[2]}function l(a,b){b=b||new q(3);var c=a[0]*a[0]+a[1]*a[1]+a[2]*a[2],d=Math.sqrt(c);return d>1e-5?(b[0]=a[0]/d,b[1]=a[1]/d,b[2]=a[2]/d):(b[0]=0,b[1]=0,b[2]=0),b}function m(a,b){return b=b||new q(3),b[0]=-a[0],b[1]=-a[1],b[2]=-a[2],b}function n(a,b){return b=b||new q(3),b[0]=a[0],b[1]=a[1],b[2]=a[2],b}function o(a,b,c){return c=c||new q(3),c[0]=a[0]*b[0],c[1]=a[1]*b[1],c[2]=a[2]*b[2],c}function p(a,b,c){return c=c||new q(3),c[0]=a[0]/b[0],c[1]=a[1]/b[1],c[2]=a[2]/b[2],c}var q=Float32Array;return{add:c,copy:n,create:b,cross:h,divide:p,divScalar:g,dot:i,lerp:e,length:j,lengthSq:k,mulScalar:f,multiply:o,negate:m,normalize:l,setDefaultType:a,subtract:d}}),c("twgl/m4",["./v3"],function(a){function b(a){VecType=a}function c(a,b){return b=b||new E(16),b[0]=-a[0],b[1]=-a[1],b[2]=-a[2],b[3]=-a[3],b[4]=-a[4],b[5]=-a[5],b[6]=-a[6],b[7]=-a[7],b[8]=-a[8],b[9]=-a[9],b[10]=-a[10],b[11]=-a[11],b[12]=-a[12],b[13]=-a[13],b[14]=-a[14],b[15]=-a[15],b}function d(a,b){return b=b||new E(16),b[0]=a[0],b[1]=a[1],b[2]=a[2],b[3]=a[3],b[4]=a[4],b[5]=a[5],b[6]=a[6],b[7]=a[7],b[8]=a[8],b[9]=a[9],b[10]=a[10],b[11]=a[11],b[12]=a[12],b[13]=a[13],b[14]=a[14],b[15]=a[15],b}function e(a){return a=a||new E(16),a[0]=1,a[1]=0,a[2]=0,a[3]=0,a[4]=0,a[5]=1,a[6]=0,a[7]=0,a[8]=0,a[9]=0,a[10]=1,a[11]=0,a[12]=0,a[13]=0,a[14]=0,a[15]=1,a}function f(a,b){if(b=b||new E(16),b===a){var c;return c=a[1],a[1]=a[4],a[4]=c,c=a[2],a[2]=a[8],a[8]=c,c=a[3],a[3]=a[12],a[12]=c,c=a[6],a[6]=a[9],a[9]=c,c=a[7],a[7]=a[13],a[13]=c,c=a[11],a[11]=a[14],a[14]=c,b}var d=a[0],e=a[1],f=a[2],g=a[3],h=a[4],i=a[5],j=a[6],k=a[7],l=a[8],m=a[9],n=a[10],o=a[11],p=a[12],q=a[13],r=a[14],s=a[15];return b[0]=d,b[1]=h,b[2]=l,b[3]=p,b[4]=e,b[5]=i,b[6]=m,b[7]=q,b[8]=f,b[9]=j,b[10]=n,b[11]=r,b[12]=g,b[13]=k,b[14]=o,b[15]=s,b}function g(a,b){b=b||new E(16);var c=a[0],d=a[1],e=a[2],f=a[3],g=a[4],h=a[5],i=a[6],j=a[7],k=a[8],l=a[9],m=a[10],n=a[11],o=a[12],p=a[13],q=a[14],r=a[15],s=m*r,t=q*n,u=i*r,v=q*j,w=i*n,x=m*j,y=e*r,z=q*f,A=e*n,B=m*f,C=e*j,D=i*f,F=k*p,G=o*l,H=g*p,I=o*h,J=g*l,K=k*h,L=c*p,M=o*d,N=c*l,O=k*d,P=c*h,Q=g*d,R=s*h+v*l+w*p-(t*h+u*l+x*p),S=t*d+y*l+B*p-(s*d+z*l+A*p),T=u*d+z*h+C*p-(v*d+y*h+D*p),U=x*d+A*h+D*l-(w*d+B*h+C*l),V=1/(c*R+g*S+k*T+o*U);return b[0]=V*R,b[1]=V*S,b[2]=V*T,b[3]=V*U,b[4]=V*(t*g+u*k+x*o-(s*g+v*k+w*o)),b[5]=V*(s*c+z*k+A*o-(t*c+y*k+B*o)),b[6]=V*(v*c+y*g+D*o-(u*c+z*g+C*o)),b[7]=V*(w*c+B*g+C*k-(x*c+A*g+D*k)),b[8]=V*(F*j+I*n+J*r-(G*j+H*n+K*r)),b[9]=V*(G*f+L*n+O*r-(F*f+M*n+N*r)),b[10]=V*(H*f+M*j+P*r-(I*f+L*j+Q*r)),b[11]=V*(K*f+N*j+Q*n-(J*f+O*j+P*n)),b[12]=V*(H*m+K*q+G*i-(J*q+F*i+I*m)),b[13]=V*(N*q+F*e+M*m-(L*m+O*q+G*e)),b[14]=V*(L*i+Q*q+I*e-(P*q+H*e+M*i)),b[15]=V*(P*m+J*e+O*i-(N*i+Q*m+K*e)),b}function h(a,b,c){c=c||new E(16);var d=a[0],e=a[1],f=a[2],g=a[3],h=a[4],i=a[5],j=a[6],k=a[7],l=a[8],m=a[9],n=a[10],o=a[11],p=a[12],q=a[13],r=a[14],s=a[15],t=b[0],u=b[1],v=b[2],w=b[3],x=b[4],y=b[5],z=b[6],A=b[7],B=b[8],C=b[9],D=b[10],F=b[11],G=b[12],H=b[13],I=b[14],J=b[15];return c[0]=d*t+e*x+f*B+g*G,c[1]=d*u+e*y+f*C+g*H,c[2]=d*v+e*z+f*D+g*I,c[3]=d*w+e*A+f*F+g*J,c[4]=h*t+i*x+j*B+k*G,c[5]=h*u+i*y+j*C+k*H,c[6]=h*v+i*z+j*D+k*I,c[7]=h*w+i*A+j*F+k*J,c[8]=l*t+m*x+n*B+o*G,c[9]=l*u+m*y+n*C+o*H,c[10]=l*v+m*z+n*D+o*I,c[11]=l*w+m*A+n*F+o*J,c[12]=p*t+q*x+r*B+s*G,c[13]=p*u+q*y+r*C+s*H,c[14]=p*v+q*z+r*D+s*I,c[15]=p*w+q*A+r*F+s*J,c}function i(a,b,c){return c=c||e(),a!==c&&(c[0]=a[0],c[1]=a[1],c[2]=a[2],c[3]=a[3],c[4]=a[4],c[5]=a[5],c[6]=a[6],c[7]=a[7],c[8]=a[8],c[9]=a[9],c[10]=a[10],c[11]=a[11]),c[12]=b[0],c[13]=b[1],c[14]=b[2],c[15]=1,c}function j(b,c){return c=c||a.create(),c[0]=b[12],c[1]=b[13],c[2]=b[14],c}function k(b,c,d){d=d||a.create();var e=4*c;return d[0]=b[e+0],d[1]=b[e+1],d[2]=b[e+2],d}function l(a,b,c,d,e){e=e||new E(16);var f=Math.tan(.5*Math.PI-.5*a),g=1/(c-d);return e[0]=f/b,e[1]=0,e[2]=0,e[3]=0,e[4]=0,e[5]=f,e[6]=0,e[7]=0,e[8]=0,e[9]=0,e[10]=(c+d)*g,e[11]=-1,e[12]=0,e[13]=0,e[14]=c*d*g*2,e[15]=0,e}function m(a,b,c,d,e,f,g){return g=g||new E(16),g[0]=2/(b-a),g[1]=0,g[2]=0,g[3]=0,g[4]=0,g[5]=2/(d-c),g[6]=0,g[7]=0,g[8]=0,g[9]=0,g[10]=-1/(f-e),g[11]=0,g[12]=(b+a)/(a-b),g[13]=(d+c)/(c-d),g[14]=-e/(e-f),g[15]=1,g}function n(a,b,c,d,e,f,g){g=g||new E(16);var h=b-a,i=d-c,j=e-f;return g[0]=2*e/h,g[1]=0,g[2]=0,g[3]=0,g[4]=0,g[5]=2*e/i,g[6]=0,g[7]=0,g[8]=(a+b)/h,g[9]=(d+c)/i,g[10]=f/j,g[11]=-1,g[12]=0,g[13]=0,g[14]=e*f/j,g[15]=0,g}function o(b,c,d,e){e=e||new E(16);var f=F,g=G,h=H;return a.normalize(a.subtract(b,c,h),h),a.normalize(a.cross(d,h,f),f),a.normalize(a.cross(h,f,g),g),e[0]=f[0],e[1]=f[1],e[2]=f[2],e[3]=0,e[4]=g[0],e[5]=g[1],e[6]=g[2],e[7]=0,e[8]=h[0],e[9]=h[1],e[10]=h[2],e[11]=0,e[12]=b[0],e[13]=b[1],e[14]=b[2],e[15]=1,e}function p(a,b){return b=b||new E(16),b[0]=1,b[1]=0,b[2]=0,b[3]=0,b[4]=0,b[5]=1,b[6]=0,b[7]=0,b[8]=0,b[9]=0,b[10]=1,b[11]=0,b[12]=a[0],b[13]=a[1],b[14]=a[2],b[15]=1,b}function q(a,b,c){c=c||new E(16);var d=b[0],e=b[1],f=b[2],g=a[0],h=a[1],i=a[2],j=a[3],k=a[4],l=a[5],m=a[6],n=a[7],o=a[8],p=a[9],q=a[10],r=a[11],s=a[12],t=a[13],u=a[14],v=a[15];return a!==c&&(c[0]=g,c[1]=h,c[2]=i,c[3]=j,c[4]=k,c[5]=l,c[6]=m,c[7]=n,c[8]=o,c[9]=p,c[10]=q,c[11]=r),c[12]=g*d+k*e+o*f+s,c[13]=h*d+l*e+p*f+t,c[14]=i*d+m*e+q*f+u,c[15]=j*d+n*e+r*f+v,c}function r(a,b){b=b||new E(16);var c=Math.cos(a),d=Math.sin(a);return b[0]=1,b[1]=0,b[2]=0,b[3]=0,b[4]=0,b[5]=c,b[6]=d,b[7]=0,b[8]=0,b[9]=-d,b[10]=c,b[11]=0,b[12]=0,b[13]=0,b[14]=0,b[15]=1,b}function s(a,b,c){c=c||new E(16);var d=a[4],e=a[5],f=a[6],g=a[7],h=a[8],i=a[9],j=a[10],k=a[11],l=Math.cos(b),m=Math.sin(b);return c[4]=l*d+m*h,c[5]=l*e+m*i,c[6]=l*f+m*j,c[7]=l*g+m*k,c[8]=l*h-m*d,c[9]=l*i-m*e,c[10]=l*j-m*f,c[11]=l*k-m*g,a!==c&&(c[0]=a[0],c[1]=a[1],c[2]=a[2],c[3]=a[3],c[12]=a[12],c[13]=a[13],c[14]=a[14],c[15]=a[15]),c}function t(a,b){b=b||new E(16);var c=Math.cos(a),d=Math.sin(a);return b[0]=c,b[1]=0,b[2]=-d,b[3]=0,b[4]=0,b[5]=1,b[6]=0,b[7]=0,b[8]=d,b[9]=0,b[10]=c,b[11]=0,b[12]=0,b[13]=0,b[14]=0,b[15]=1,b}function u(a,b,c){c=c||new E(16);var d=a[0],e=a[1],f=a[2],g=a[3],h=a[8],i=a[9],j=a[10],k=a[11],l=Math.cos(b),m=Math.sin(b);return c[0]=l*d-m*h,c[1]=l*e-m*i,c[2]=l*f-m*j,c[3]=l*g-m*k,c[8]=l*h+m*d,c[9]=l*i+m*e,c[10]=l*j+m*f,c[11]=l*k+m*g,a!==c&&(c[4]=a[4],c[5]=a[5],c[6]=a[6],c[7]=a[7],c[12]=a[12],c[13]=a[13],c[14]=a[14],c[15]=a[15]),c}function v(a,b){b=b||new E(16);var c=Math.cos(a),d=Math.sin(a);return b[0]=c,b[1]=d,b[2]=0,b[3]=0,b[4]=-d,b[5]=c,b[6]=0,b[7]=0,b[8]=0,b[9]=0,b[10]=1,b[11]=0,b[12]=0,b[13]=0,b[14]=0,b[15]=1,b}function w(a,b,c){c=c||new E(16);var d=a[0],e=a[1],f=a[2],g=a[3],h=a[4],i=a[5],j=a[6],k=a[7],l=Math.cos(b),m=Math.sin(b);return c[0]=l*d+m*h,c[1]=l*e+m*i,c[2]=l*f+m*j,c[3]=l*g+m*k,c[4]=l*h-m*d,c[5]=l*i-m*e,c[6]=l*j-m*f,c[7]=l*k-m*g,a!==c&&(c[8]=a[8],c[9]=a[9],c[10]=a[10],c[11]=a[11],c[12]=a[12],c[13]=a[13],c[14]=a[14],c[15]=a[15]),c}function x(a,b,c){c=c||new E(16);var d=a[0],e=a[1],f=a[2],g=Math.sqrt(d*d+e*e+f*f);d/=g,e/=g,f/=g;var h=d*d,i=e*e,j=f*f,k=Math.cos(b),l=Math.sin(b),m=1-k;return c[0]=h+(1-h)*k,c[1]=d*e*m+f*l,c[2]=d*f*m-e*l,c[3]=0,c[4]=d*e*m-f*l,c[5]=i+(1-i)*k,c[6]=e*f*m+d*l,c[7]=0,c[8]=d*f*m+e*l,c[9]=e*f*m-d*l,c[10]=j+(1-j)*k,c[11]=0,c[12]=0,c[13]=0,c[14]=0,c[15]=1,c}function y(a,b,c,d){d=d||new E(16);var e=b[0],f=b[1],g=b[2],h=Math.sqrt(e*e+f*f+g*g);e/=h,f/=h,g/=h;var i=e*e,j=f*f,k=g*g,l=Math.cos(c),m=Math.sin(c),n=1-l,o=i+(1-i)*l,p=e*f*n+g*m,q=e*g*n-f*m,r=e*f*n-g*m,s=j+(1-j)*l,t=f*g*n+e*m,u=e*g*n+f*m,v=f*g*n-e*m,w=k+(1-k)*l,x=a[0],y=a[1],z=a[2],A=a[3],B=a[4],C=a[5],D=a[6],F=a[7],G=a[8],H=a[9],I=a[10],J=a[11];return d[0]=o*x+p*B+q*G,d[1]=o*y+p*C+q*H,d[2]=o*z+p*D+q*I,d[3]=o*A+p*F+q*J,d[4]=r*x+s*B+t*G,d[5]=r*y+s*C+t*H,d[6]=r*z+s*D+t*I,d[7]=r*A+s*F+t*J,d[8]=u*x+v*B+w*G,d[9]=u*y+v*C+w*H,d[10]=u*z+v*D+w*I,d[11]=u*A+v*F+w*J,a!==d&&(d[12]=a[12],d[13]=a[13],d[14]=a[14],d[15]=a[15]),d}function z(a,b){return b=b||new E(16),b[0]=a[0],b[1]=0,b[2]=0,b[3]=0,b[4]=0,b[5]=a[1],b[6]=0,b[7]=0,b[8]=0,b[9]=0,b[10]=a[2],b[11]=0,b[12]=0,b[13]=0,b[14]=0,b[15]=1,b}function A(a,b,c){c=c||new E(16);var d=b[0],e=b[1],f=b[2];return c[0]=d*a[0],c[1]=d*a[1],c[2]=d*a[2],c[3]=d*a[3],c[4]=e*a[4],c[5]=e*a[5],c[6]=e*a[6],c[7]=e*a[7],c[8]=f*a[8],c[9]=f*a[9],c[10]=f*a[10],c[11]=f*a[11],a!==c&&(c[12]=a[12],c[13]=a[13],c[14]=a[14],c[15]=a[15]),a}function B(b,c,d){d=d||a.create();var e=c[0],f=c[1],g=c[2],h=e*b[3]+f*b[7]+g*b[11]+b[15];return d[0]=(e*b[0]+f*b[4]+g*b[8]+b[12])/h,d[1]=(e*b[1]+f*b[5]+g*b[9]+b[13])/h,d[2]=(e*b[2]+f*b[6]+g*b[10]+b[14])/h,d}function C(b,c,d){d=d||a.create();var e=c[0],f=c[1],g=c[2];return d[0]=e*b[0]+f*b[4]+g*b[8],d[1]=e*b[1]+f*b[5]+g*b[9],d[2]=e*b[2]+f*b[6]+g*b[10],d}function D(b,c,d){d=d||a.create();var e=g(b),f=c[0],h=c[1],i=c[2];return d[0]=f*e[0]+h*e[1]+i*e[2],d[1]=f*e[4]+h*e[5]+i*e[6],d[2]=f*e[8]+h*e[9]+i*e[10],d}var E=Float32Array,F=a.create(),G=a.create(),H=a.create();return{axisRotate:y,axisRotation:x,create:e,copy:d,frustum:n,getAxis:k,getTranslation:j,identity:e,inverse:g,lookAt:o,multiply:h,negate:c,ortho:m,perspective:l,rotateX:s,rotateY:u,rotateZ:w,rotationX:r,rotationY:t,rotationZ:v,scale:A,scaling:z,setDefaultType:b,setTranslation:i,transformDirection:C,transformNormal:D,transformPoint:B,translate:q,translation:p,transpose:f}}),c("twgl/primitives",["./twgl","./m4","./v3"],function(a,b,c){function d(a,b){var c=0;return a.push=function(){for(var b=0;b<arguments.length;++b){var d=arguments[b];if(d instanceof Array||d.buffer&&d.buffer instanceof ArrayBuffer)for(var e=0;e<d.length;++e)a[c++]=d[e];else a[c++]=d}},a.reset=function(a){c=a||0},a.numComponents=b,Object.defineProperty(a,"numElements",{get:function(){return this.length/this.numComponents|0}}),a}function e(a,b,c){var e=c||Float32Array;return d(new e(a*b),a)}function f(a){return"indices"!==a}function g(a){function b(b){for(var f=a[b],h=f.numComponents,i=e(h,g,f.constructor),j=0;g>j;++j)for(var k=c[j],l=k*h,m=0;h>m;++m)i.push(f[l+m]);d[b]=i}var c=a.indices,d={},g=c.length;return Object.keys(a).filter(f).forEach(b),d}function h(a){if(a.indices)throw"can't flatten normals of indexed vertices. deindex them first";for(var b=a.normal,c=b.length,d=0;c>d;d+=9){var e=b[d+0],f=b[d+1],g=b[d+2],h=b[d+3],i=b[d+4],j=b[d+5],k=b[d+6],l=b[d+7],m=b[d+8],n=e+h+k,o=f+i+l,p=g+j+m,q=Math.sqrt(n*n+o*o+p*p);n/=q,o/=q,p/=q,b[d+0]=n,b[d+1]=o,b[d+2]=p,b[d+3]=n,b[d+4]=o,b[d+5]=p,b[d+6]=n,b[d+7]=o,b[d+8]=p}return a}function i(a,b,c){for(var d=a.length,e=new Float32Array(3),f=0;d>f;f+=3)c(b,[a[f],a[f+1],a[f+2]],e),a[f]=e[0],a[f+1]=e[1],a[f+2]=e[2]}function j(a,b,d){d=d||c.create();var e=b[0],f=b[1],g=b[2];return d[0]=e*a[0]+f*a[1]+g*a[2],d[1]=e*a[4]+f*a[5]+g*a[6],d[2]=e*a[8]+f*a[9]+g*a[10],d}function k(a,c){return i(a,c,b.transformDirection),a}function l(a,c){return i(a,b.inverse(c),j),a}function m(a,c){return i(a,c,b.transformPoint),a}function n(a,b){return Object.keys(a).forEach(function(c){var d=a[c];c.indexOf("pos")>=0?m(d,b):c.indexOf("tan")>=0||c.indexOf("binorm")>=0?k(d,b):c.indexOf("norm")>=0&&l(d,b)}),a}function o(a,b,c){return a=a||2,b=b||0,c=c||0,a*=.5,{position:{numComponents:2,data:[b+-1*a,c+-1*a,b+1*a,c+-1*a,b+-1*a,c+1*a,b+1*a,c+1*a]},normal:[0,0,1,0,0,1,0,0,1,0,0,1],texcoord:[0,0,1,0,0,1,1,1],indices:[0,1,2,2,1,3]}}function p(a,c,d,f,g){a=a||1,c=c||1,d=d||1,f=f||1,g=g||b.identity();for(var h=(d+1)*(f+1),i=e(3,h),j=e(3,h),k=e(2,h),l=0;f>=l;l++)for(var m=0;d>=m;m++){var o=m/d,p=l/f;i.push(a*o-.5*a,0,c*p-.5*c),j.push(0,1,0),k.push(o,p)}for(var q=d+1,r=e(3,d*f*2,Uint16Array),l=0;f>l;l++)for(var m=0;d>m;m++)r.push((l+0)*q+m,(l+1)*q+m,(l+0)*q+m+1),r.push((l+1)*q+m,(l+1)*q+m+1,(l+0)*q+m+1);var s=n({position:i,normal:j,texcoord:k,indices:r},g);return s}function q(a,b,c,d,f,g,h){if(0>=b||0>=c)throw Error("subdivisionAxis and subdivisionHeight must be > 0");d=d||0,f=f||Math.PI,g=g||0,h=h||2*Math.PI;for(var i=f-d,j=h-g,k=(b+1)*(c+1),l=e(3,k),m=e(3,k),n=e(2,k),o=0;c>=o;o++)for(var p=0;b>=p;p++){var q=p/b,r=o/c,s=j*q,t=i*r,u=Math.sin(s),v=Math.cos(s),w=Math.sin(t),x=Math.cos(t),y=v*w,z=x,A=u*w;l.push(a*y,a*z,a*A),m.push(y,z,A),n.push(1-q,r)}for(var B=b+1,C=e(3,b*c*2,Uint16Array),p=0;b>p;p++)for(var o=0;c>o;o++)C.push((o+0)*B+p,(o+0)*B+p+1,(o+1)*B+p),C.push((o+1)*B+p,(o+0)*B+p+1,(o+1)*B+p+1);return{position:l,normal:m,
texcoord:n,indices:C}}function r(a){a=a||1;for(var b=a/2,c=[[-b,-b,-b],[+b,-b,-b],[-b,+b,-b],[+b,+b,-b],[-b,-b,+b],[+b,-b,+b],[-b,+b,+b],[+b,+b,+b]],d=[[1,0,0],[-1,0,0],[0,1,0],[0,-1,0],[0,0,1],[0,0,-1]],f=[[1,0],[0,0],[0,1],[1,1]],g=24,h=e(3,g),i=e(3,g),j=e(2,g),k=e(3,12,Uint16Array),l=0;6>l;++l){for(var m=D[l],n=0;4>n;++n){var o=c[m[n]],p=d[l],q=f[n];h.push(o),i.push(p),j.push(q)}var r=4*l;k.push(r+0,r+1,r+2),k.push(r+0,r+2,r+3)}return{position:h,normal:i,texcoord:j,indices:k}}function s(a,b,c,d,f,g,h){if(3>d)throw Error("radialSubdivisions must be 3 or greater");if(1>f)throw Error("verticalSubdivisions must be 1 or greater");for(var i=void 0===g?!0:g,j=void 0===h?!0:h,k=(i?2:0)+(j?2:0),l=(d+1)*(f+1+k),m=e(3,l),n=e(3,l),o=e(2,l),p=e(3,d*(f+k)*2,Uint16Array),q=d+1,r=Math.atan2(a-b,c),s=Math.cos(r),t=Math.sin(r),u=i?-2:0,v=f+(j?2:0),w=u;v>=w;++w){var x,y=w/f,z=c*y;0>w?(z=0,y=1,x=a):w>f?(z=c,y=1,x=b):x=a+(b-a)*(w/f),(-2===w||w===f+2)&&(x=0,y=0),z-=c/2;for(var A=0;q>A;++A){var B=Math.sin(A*Math.PI*2/d),C=Math.cos(A*Math.PI*2/d);m.push(B*x,z,C*x),n.push(0>w||w>f?0:B*s,0>w?-1:w>f?1:t,0>w||w>f?0:C*s),o.push(A/d,1-y)}}for(var w=0;f+k>w;++w)for(var A=0;d>A;++A)p.push(q*(w+0)+0+A,q*(w+0)+1+A,q*(w+1)+1+A),p.push(q*(w+0)+0+A,q*(w+1)+1+A,q*(w+1)+0+A);return{position:m,normal:n,texcoord:o,indices:p}}function t(a,b){b=b||[];for(var c=[],d=0;d<a.length;d+=4){var e=a[d],f=a.slice(d+1,d+4);f.push.apply(f,b);for(var g=0;e>g;++g)c.push.apply(c,f)}return c}function u(){var a=[0,0,0,0,150,0,30,0,0,0,150,0,30,150,0,30,0,0,30,0,0,30,30,0,100,0,0,30,30,0,100,30,0,100,0,0,30,60,0,30,90,0,67,60,0,30,90,0,67,90,0,67,60,0,0,0,30,30,0,30,0,150,30,0,150,30,30,0,30,30,150,30,30,0,30,100,0,30,30,30,30,30,30,30,100,0,30,100,30,30,30,60,30,67,60,30,30,90,30,30,90,30,67,60,30,67,90,30,0,0,0,100,0,0,100,0,30,0,0,0,100,0,30,0,0,30,100,0,0,100,30,0,100,30,30,100,0,0,100,30,30,100,0,30,30,30,0,30,30,30,100,30,30,30,30,0,100,30,30,100,30,0,30,30,0,30,60,30,30,30,30,30,30,0,30,60,0,30,60,30,30,60,0,67,60,30,30,60,30,30,60,0,67,60,0,67,60,30,67,60,0,67,90,30,67,60,30,67,60,0,67,90,0,67,90,30,30,90,0,30,90,30,67,90,30,30,90,0,67,90,30,67,90,0,30,90,0,30,150,30,30,90,30,30,90,0,30,150,0,30,150,30,0,150,0,0,150,30,30,150,30,0,150,0,30,150,30,30,150,0,0,0,0,0,0,30,0,150,30,0,0,0,0,150,30,0,150,0],b=[.22,.19,.22,.79,.34,.19,.22,.79,.34,.79,.34,.19,.34,.19,.34,.31,.62,.19,.34,.31,.62,.31,.62,.19,.34,.43,.34,.55,.49,.43,.34,.55,.49,.55,.49,.43,0,0,1,0,0,1,0,1,1,0,1,1,0,0,1,0,0,1,0,1,1,0,1,1,0,0,1,0,0,1,0,1,1,0,1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,0],c=t([18,0,0,1,18,0,0,-1,6,0,1,0,6,1,0,0,6,0,-1,0,6,1,0,0,6,0,1,0,6,1,0,0,6,0,-1,0,6,1,0,0,6,0,-1,0,6,-1,0,0]),d=t([18,200,70,120,18,80,70,200,6,70,200,210,6,200,200,70,6,210,100,70,6,210,160,70,6,70,180,210,6,100,70,210,6,76,210,100,6,140,210,80,6,90,130,110,6,160,160,220],[255]),f=a.length/3,g={position:e(3,f),texcoord:e(2,f),normal:e(3,f),color:e(4,f,Uint8Array),indices:e(3,f/3,Uint16Array)};g.position.push(a),g.texcoord.push(b),g.normal.push(c),g.color.push(d);for(var h=0;f>h;++h)g.indices.push(h);return g}function v(a,b,d,f,g,h,i){function j(a,b,c){return a+(b-a)*c}function k(b,d,e,i,k,l){for(var o=0;g>=o;o++){var s=d/(m-1),t=o/g,u=2*(s-.5),v=(h+t*n)*Math.PI,w=Math.sin(v),x=Math.cos(v),y=j(a,b,w),z=u*f,A=x*a,B=w*y;p.push(z,A,B);var C=c.add(c.multiply([0,w,x],e),i);q.push(C),r.push(s*k+l,t)}}function l(a,b){for(var c=0;g>c;++c)u.push(a+c+0,a+c+1,b+c+0),u.push(a+c+1,b+c+1,b+c+0)}if(0>=g)throw Error("subdivisionDown must be > 0");h=h||0,i=i||1;for(var m=2,n=i-h,o=2*(g+1)*(2+m),p=e(3,o),q=e(3,o),r=e(2,o),s=0;m>s;s++){var t=2*(s/(m-1)-.5);k(b,s,[1,1,1],[0,0,0],1,0),k(b,s,[0,0,0],[t,0,0],0,0),k(d,s,[1,1,1],[0,0,0],1,0),k(d,s,[0,0,0],[t,0,0],0,1)}var u=e(3,2*g*(2+m),Uint16Array),v=g+1;return l(0*v,4*v),l(5*v,7*v),l(6*v,2*v),l(3*v,1*v),{position:p,normal:q,texcoord:r,indices:u}}function w(a,b,c,d,e,f){return s(a,a,b,c,d,e,f)}function x(a,b,c,d,f,g){if(3>c)throw Error("radialSubdivisions must be 3 or greater");if(3>d)throw Error("verticalSubdivisions must be 3 or greater");f=f||0,g=g||2*Math.PI,range=g-f;for(var h=c+1,i=d+1,j=h*i,k=e(3,j),l=e(3,j),m=e(2,j),n=e(3,c*d*2,Uint16Array),o=0;i>o;++o)for(var p=o/d,q=p*Math.PI*2,r=Math.sin(q),s=a+r*b,t=Math.cos(q),u=t*b,v=0;h>v;++v){var w=v/c,x=f+w*range,y=Math.sin(x),z=Math.cos(x),A=y*s,B=z*s,C=y*r,D=z*r;k.push(A,u,B),l.push(C,t,D),m.push(w,1-p)}for(var o=0;d>o;++o)for(var v=0;c>v;++v){var E=1+v,F=1+o;n.push(h*o+v,h*F+v,h*o+E),n.push(h*F+v,h*F+E,h*o+E)}return{position:k,normal:l,texcoord:m,indices:n}}function y(a,b,c,d,f){if(3>b)throw Error("divisions must be at least 3");c=c?c:1,f=f?f:1,d=d?d:0;for(var g=(b+1)*(c+1),h=e(3,g),i=e(3,g),j=e(2,g),k=e(3,c*b*2,Uint16Array),l=0,m=a-d,n=0;c>=n;++n){for(var o=d+m*Math.pow(n/c,f),p=0;b>=p;++p){var q=2*Math.PI*p/b,r=o*Math.cos(q),s=o*Math.sin(q);if(h.push(r,0,s),i.push(0,1,0),j.push(1-p/b,n/c),n>0&&p!==b){var t=l+(p+1),u=l+p,v=l+p-b,w=l+(p+1)-b;k.push(t,u,v),k.push(t,v,w)}}l+=b+1}return{position:h,normal:i,texcoord:j,indices:k}}function z(a){return Math.random()*a|0}function A(a,b){b=b||{};var c=a.position.numElements,d=e(4,c,Uint8Array),f=b.rand||function(a,b){return 3>b?z(256):255};if(a.color=d,a.indices)for(var g=0;c>g;++g)d.push(f(g,0),f(g,1),f(g,2),f(g,3));else for(var h=b.vertsPerColor||3,i=c/h,g=0;i>g;++g)for(var j=[f(g,0),f(g,1),f(g,2),f(g,3)],k=0;h>k;++k)d.push(j);return a}function B(b){return function(c){var d=b.apply(this,Array.prototype.slice.call(arguments,1));return a.createBuffersFromArrays(c,d)}}function C(b){return function(c){var d=b.apply(null,Array.prototype.slice.call(arguments,1));return a.createBufferInfoFromArrays(c,d)}}var D=[[3,7,5,1],[6,2,0,4],[6,7,3,2],[0,1,5,4],[7,6,4,5],[2,3,1,0]];return{create3DFBufferInfo:C(u),create3DFBuffers:B(u),create3DFVertices:u,createAugmentedTypedArray:e,createCubeBufferInfo:C(r),createCubeBuffers:B(r),createCubeVertices:r,createPlaneBufferInfo:C(p),createPlaneBuffers:B(p),createPlaneVertices:p,createSphereBufferInfo:C(q),createSphereBuffers:B(q),createSphereVertices:q,createTruncatedConeBufferInfo:C(s),createTruncatedConeBuffers:B(s),createTruncatedConeVertices:s,createXYQuadBufferInfo:C(o),createXYQuadBuffers:B(o),createXYQuadVertices:o,createCresentBufferInfo:C(v),createCresentBuffers:B(v),createCresentVertices:v,createCylinderBufferInfo:C(w),createCylinderBuffers:B(w),createCylinderVertices:w,createTorusBufferInfo:C(x),createTorusBuffers:B(x),createTorusVertices:x,createDiscBufferInfo:C(y),createDiscBuffers:B(y),createDiscVertices:y,deindexVertices:g,flattenNormals:h,makeRandomVertexColors:A,reorientDirections:k,reorientNormals:l,reorientPositions:m,reorientVertices:n}}),c("main",["twgl/twgl","twgl/m4","twgl/v3","twgl/primitives"],function(a,b,c,d){return a.m4=b,a.v3=c,a.primitives=d,a}),b(["main"],function(a){return a},void 0,!0),c("build/js/twgl-includer-full",function(){}),b("main")});

function resizeTifyboxHandler()
{
   // We first need to reset the height of the canvas... or else we will have a
   // self-feeding height...

   $("#tbObj").css({"height":"10px"});
   var topY = $("#tifyDiv").offset().top;
   var bottomY = $(window).height();
   var h = (bottomY - topY - 10);

   // Allow no smaller than 300, but as large as can go

   h = Math.max(h,300);
   $("#tbObj").css({"height":h+"px"});
}

function isCanvasSupported()
{
   var elem = document.createElement('canvas');
   return !!(elem.getContext && elem.getContext('2d'));
}

function isWebglSupported()
{
   var elem = document.createElement('canvas');
   return !!(elem.getContext('webgl') || elem.getContext('experimental-webgl'));
}

function intializeTifyboxGL(containerDivID, pixelsPerBlock, blocksPerFaceAcross, faceImgPaths, faceImgNames)
{
   // Determine Compatibility (canvas)
   if (!isCanvasSupported() || !isWebglSupported())
   {
      document.getElementById(containerDivID).innerHTML =
      '<p>tifyBox.com</p><p>Sorry! Your browser isn\'t supported. Please try the \
      following:</p><ul><li>Update browser to latest version.</li><li>Try one of \
      these browsers: <a href="http://www.google.com/chrome/">Chrome</a>, \
      <a href="http://www.mozilla.org/firefox/">FireFox</a>, \
      <a href="http://www.apple.com/safari/">Safari</a>, \
      <a href="http://windows.microsoft.com/en-us/internet-explorer/download-ie">IE (v.9+)</a>, \
      <a href="http://www.opera.com">Opera</a></li></ul>';

      return false;
   }
   else
   {
      document.getElementById(containerDivID).innerHTML =
      '<div class="" id="canvasDiv"><canvas id="tbObj" class="center-block" \
      style=" background-color:#FDFDFD; width:100%;"></canvas></div>';

      $( window ).resize(resizeTifyboxHandler);
      resizeTifyboxHandler();

      return (new tifyboxGL(document.getElementById('tbObj'),
                            pixelsPerBlock,
                            blocksPerFaceAcross,
                            faceImgPaths,
                            faceImgNames));
   }
}

// -------------------------------------------------------------------------------------------------
// The tifybox Class
// -------------------------------------------------------------------------------------------------

function tifyboxGL(canvasID, pixelsPerBlock, blocksPerFaceAcross, faceImgPaths, faceImgNames)
{
   // Input Validation

   if (canvasID === undefined || canvasID === null)
   {
      alert("Error: Undefined Canvas");
      return;
   }
   else if (pixelsPerBlock === undefined || pixelsPerBlock === null)
   {
      alert("Error: Undefined pixelsPerBlock");
      return;
   }
   else if (blocksPerFaceAcross === undefined || blocksPerFaceAcross === null)
   {
      alert("Error: Undefined blocksPerFaceAcross");
      return;
   }
   if (faceImgPaths === undefined || faceImgPaths === null)
   {
      alert("Error: Undefined faceImgPaths");
      return;
   }
   else if (faceImgNames === undefined || faceImgNames === null)
   {
      alert("Error: Undefined faceImgNames");
      return;
   }

   //***********************************************************************************************
   //Private Constants
   //***********************************************************************************************
   var DIAMETER      = 1000.0;                      //Cube Diameter
   var RADIUS        = (DIAMETER / 2.0);            //Cube Radius
   var MAX_XY        = DIAMETER - 1;                //Coords start at 0,0
   var SPHERE_RADIUS = Math.pow(3.0,0.5) * RADIUS;  //Radius of sphere that encompasses the cube
   var UNUSED_ALPHA  = 252;                         //
   var GUESS_TIERS   = 5;                           //Squares from clicked pixel that we look for a valid pixel
   var FOV           = 0.52359878;                  //Field of View
   var ZOOM_HI       = 6000.0;                      //Max distance we'll allow the user to zoom out
   var ZOOM_LO       = 875.0;                       //Min distance before going into cube's corner
   var NEAR_CLIP     = 0.5;                         //
   var FAR_CLIP      = ZOOM_HI + 1000.0;            //
   var ALPHAS        = [[0.003922,1],[0.023529,6],[.043137,11],[0.062745,16],[0.082353,21],[0.101961,26]];
   var FACE_URLS     = [];
   var FACE_NAMES    = [];
   var IMAGE_RGBA    = [[],[],[],[],[],[]];

   var PIX_PER_BLOCK_LENGTH = 0;
   var BLOCKS_PER_FACE_LENGTH = 0;

   //***********************************************************************************************
   //Private Globals Vars
   //***********************************************************************************************
   var graphics = {canvas:null,gl:null,m4:twgl.m4,v3:twgl.v3,drawObjects:[],texFB:null,tex:null,renderToTexture:false,initial:true,time:0};
   var matrices = {view:null,projection:null,rotation:null,translation:null,initialTransforms:null};
   var posJump = {active:false,verifyStep:false,face:0,x:0,y:0,rotation0:{axis:[0,1,0],angle:0,updatesLeft:0},rotation1:{axis:[0,1,0],angle:0,updatesLeft:0},translate:{vec:[0,0,0],updatesLeft:0},JUMP_INTERVAL:20};
   var rotDrag = {ROTATE_THRESHOLD_CLICK:5,ROTATE_THRESHOLD_TOUCH:10};
   var rotThrow = {speed:0,axis:[0,1,0],delta0:0,delta1:0,degration:0,MAX_SPEED:0.2,MAX_DEGRATION:0.001,SPEED_DIVIDER:25.0,DEGRATION_DIVIDER:15.0,THROW_THRESHOLD_MS:300,THROW_THRESHOLD:30};
   var curserInfo = {isAvailable:true,isDown:false,isDragging:false,isAfterRotate:false,clickedPos:[0,0],hisory:[]};
   var touchInfo = {justFinishedTwo:false,isTranslating:false,twoFingers:false,isScaling:false,lastScaled:0,hisory:[],PINCH_DIVIDER:10.0,PULSE_ERROR:5};
   var movementType = {rotate:true,translate:false};
   var scalarVec3D = [0,0,0];
   var zoomPercent = 70;
   var renderTextureMutex = true;
   var selectionInfo = {x:0,y:0,face:0,faceName:FACE_NAMES[0],pixelNum:0,validURL:false,updated:false,shiftDown:false};
   var selectionMutex = true;

   //***********************************************************************************************
   //Public Functions
   //***********************************************************************************************
   //-----------------------------------------------------------------------------------------------
   //Name:
   // getDominateSide
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // ...
   //
   //About:
   // Given a sample returned from the tifybox obj., determines which side of the cube is most
   // exposed.
   //
   //-----------------------------------------------------------------------------------------------

   this.getDominateSide = function(sample)
   {
      var domIndex = 0;
      var domValue = Math.abs(sample[0]);

      for (var i = 1; i < 6; i++)
      {
         if (Math.abs(sample[i]) < domValue)
         {
            domIndex = i;
            domValue = Math.abs(sample[i]);
         }
      }

      return domIndex;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getBlockNumberFromPixelXY
   //
   //Arguments:
   // Pixel x,y coords
   //
   //Output:
   // void
   //
   //Returns:
   // Block Number
   //
   //About:
   // Converts an x,y pixel coord into a block number
   //
   //-----------------------------------------------------------------------------------------------

   this.getBlockNumberFromPixelXY = function(x, y)
   {
      var coords = pixelXY_to_blockXY(x,y);
      return getBlockNumberFromBlockXY(coords.x,coords.y);
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getBlockNumberFromBlockXY
   //
   //Arguments:
   // Block x,y coords
   //
   //Output:
   // void
   //
   //Returns:
   // Block Number
   //
   //About:
   // Converts an x,y block coord into a block number
   //
   //-----------------------------------------------------------------------------------------------

   this.getBlockNumberFromBlockXY = function(x, y)
   {
      return ((y - 1) * BLOCKS_PER_FACE_LENGTH) + x;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // pixelXY_to_blockXY
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // Coord object
   //
   //About:
   // Converts an x,y pixel coord into a block coord
   //
   //-----------------------------------------------------------------------------------------------

   this.pixelXY_to_blockXY = function(x, y)
   {
      var retCoord = {x:0, y:0};

      retCoord.x = parseInt(x / PIX_PER_BLOCK_LENGTH) + 1;
      retCoord.y = parseInt(y / PIX_PER_BLOCK_LENGTH) + 1;

      return retCoord;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // blockXY_to_pixelXY
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // Coord object
   //
   //About:
   // Converts an x,y block coord into a pixel coord
   //
   //-----------------------------------------------------------------------------------------------

   this.blockXY_to_pixelXY = function(x, y)
   {
      var retCoord = {x:0, y:0};

      retCoord.x = (x - 1) * PIX_PER_BLOCK_LENGTH;
      retCoord.y = (y - 1) * PIX_PER_BLOCK_LENGTH;

      return retCoord;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getSelectionInfo
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // Object which contains information concerning the last cube click
   //
   //About:
   //
   //-----------------------------------------------------------------------------------------------

   this.getSelectionInfo = function()
   {
      return selectionInfo;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // resetSelectionUpdate
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Pixel selection will continue to be updated. This is simply a flag for the client to use to
   // determine if there has been a selection update since the last time they pulled.
   //
   //-----------------------------------------------------------------------------------------------

   this.resetSelectionUpdate = function()
   {
      selectionInfo.updated = false;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // centerFace
   //
   //Arguments:
   // face - Cube Face (int)
   //
   //Output:
   // void
   //
   //Returns:
   // true upon valid inputs
   //
   //About:
   // Centers the cube with the input face facing towards us.
   //
   //-----------------------------------------------------------------------------------------------

   this.centerFace = function(face)
   {
      var val = parseFloat(face);
      if (!validateInput(val,0,0))
      {
         return false;
      }
      transformTo(val,RADIUS,RADIUS);
      return true;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // jumpTo
   //
   //Arguments:
   // face - Cube Face (int)
   // x, y - coordinates
   //
   //Output:
   // void
   //
   //Returns:
   // true upon valid inputs
   //
   //About:
   // Sets in motion the transformation of the cube to center the provided coord onto the canvas.
   //
   //-----------------------------------------------------------------------------------------------

   this.jumpTo = function(face, x, y)
   {
      var val = parseFloat(x);
      var val2 = parseFloat(y);
      var val3 = parseFloat(face);
      var success = false;

      if (validateInput(val3,val,val2))
      {
         transformTo(val3,val,val2);
         success = true;
      }

      return success;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getExposureSample
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // Array where each index represents the angle in radians between a vector coming out of the
   // screen and a vector normal to the cube face. This means that the smaller the value, the more
   // exposed that face is to the user.
   //
   //About:
   // This function is used to determine which cube face is most exposed to the viewer. (Which one
   // is facing forward.)
   //
   //-----------------------------------------------------------------------------------------------

   this.getExposureSample = function()
   {
      return sampleExposureOfEachCubeFace();
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // stimulateClickEvent
   //
   //Arguments:
   // f - Cube Face (int)
   // x, y - coordinates
   //
   //Output:
   // void
   //
   //Returns:
   // Returns the selectionInfo object, which contains information concerning the click.
   //
   //About:
   // This is used as a way to bypass an actual physical click on the cube.
   //
   //-----------------------------------------------------------------------------------------------

   this.stimulateClickEvent = function(f, x, y)
   {
      var val = parseFloat(x);
      var val2 = parseFloat(y);
      var val3 = parseFloat(f);

      if (validateInput(val3,val,val2) && selectionMutex)
      {
         selectionMutex = false;
         selectionInfo.shiftDown = false;
         selectionInfo.x = val;
         selectionInfo.y = val2;
         selectionInfo.face = val3;
         selectionInfo.faceName = FACE_NAMES[val3];
         selectionInfo.pixelNum = (val2 * DIAMETER + val);
         var buffer = IMAGE_RGBA[selectionInfo.face];
         var alpha = buffer[selectionInfo.pixelNum];

         if (!is_UNUSED_ALPHA(alpha)) //Valid Selection
         {
            selectionInfo.validURL = true;
         }
         else
         {
            selectionInfo.validURL = false;
         }

         selectionInfo.updated = true;
         selectionMutex = true;

         return selectionInfo;
      }
      else
      {
         return false;
      }
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // hasAnAssignedURL
   //
   //Arguments:
   // f - Cube Face (int)
   // x, y - coordinates
   //
   //Output:
   // void
   //
   //Returns:
   // Returns true if there is an assigned URL to the coords
   //
   //About:
   // Check is entirely dependent on the face image alpha values...
   //
   //-----------------------------------------------------------------------------------------------

   this.hasAnAssignedURL = function(f, x, y)
   {
      var val = parseFloat(x);
      var val2 = parseFloat(y);
      var val3 = parseFloat(f);
      if (!validateInput(val3,val,val2))
      {
         return false;
      }
      var buffer = IMAGE_RGBA[val3];
      var alpha = buffer[(val2 * DIAMETER + val)];

      return !(is_UNUSED_ALPHA(alpha));
   }

   //***********************************************************************************************
   //Private Functions
   //***********************************************************************************************
   //-----------------------------------------------------------------------------------------------
   //Name:
   // is_UNUSED_ALPHA
   //
   //Arguments:
   // alpha - The alpha value (0 - 255) to be compared
   //
   //Output:
   // void
   //
   //Returns:
   // True if passed in alpha is equal to, or only off by a margin of 1, to constant UNUSED_ALPHA
   //
   //About:
   // Margin of 1 is used due to inconsistencies in image generation, manipulation, etc.
   //
   //-----------------------------------------------------------------------------------------------

   function is_UNUSED_ALPHA(alpha)
   {
      if (Math.abs(UNUSED_ALPHA - alpha) <= 1)
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // validateInput
   //
   //Arguments:
   // f - Cube Face (int)
   // x, y - coordinates
   //
   //Output:
   // void
   //
   //Returns:
   // True if arguments are within acceptable ranges
   //
   //About:
   // void
   //
   //-----------------------------------------------------------------------------------------------

   function validateInput(f, x, y)
   {
      if (isNaN(x) || isNaN(y) || isNaN(f) || (x === null) || (y === null) || (f === null) ||
         (x < 0) || (y < 0) || (f < 0) || (x > MAX_XY) || (y > MAX_XY) || (f > 5))
      {
         return false;
      }
      else
      {
         return true;
      }
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // changeZoomByNPercentDo
   //
   //Arguments:
   // N - Percent to change zoom by (-100 to 100)
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Modified zoom percent will always result in a range of 0 to 100
   //
   //-----------------------------------------------------------------------------------------------

   function changeZoomByNPercentDo(N)
   {
      var zoom = zoomPercent + N;
      if (zoom < 0)
      {
         zoom = 0;
      }
      else if (zoom > 100)
      {
         zoom = 100;
      }
      zoomPercent = zoom;

      //Get the zoom factor and update view matrix
      zoom = ZOOM_HI - (((ZOOM_HI - ZOOM_LO)/100) * zoomPercent);
      graphics.m4.lookAt([0,0,zoom], [0,0,0], [0,1,0], matrices.view);
      graphics.m4.inverse(matrices.view, matrices.view);

      //Update scalar values
      getTranslateAndRotateScalars();
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // extractLocalCopyOfAlphaValues
   //
   //Arguments:
   // num  - Offset into array of cube face image data. (0 - 5)
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Loads the cube's face RGBA image into array, extracts the alpha values, and stores them
   // locally so that we don't have to ask the server for pixel information if the pixel is  marked
   // as not occupied.
   //
   //-----------------------------------------------------------------------------------------------

   function extractLocalCopyOfAlphaValues(num)
   {
      var canvas = document.createElement('canvas');
      canvas.width = DIAMETER;
      canvas.height = DIAMETER;
      var ctx = canvas.getContext('2d');
      var imageObj = new Image();
      imageObj.onload = function()
      {
         ctx.drawImage(this, 0, 0);
         var imgdata = ctx.getImageData(0,0,DIAMETER,DIAMETER);

         //Extract the alpha values
         for (var i = 3; i < (imgdata.data).length; i += 4)
         {
            (IMAGE_RGBA[num]).push((imgdata.data)[i]);
         }

         imgdata = null;
         //canvas.remove(); //NOT IE COMPATIBLE
         num += 1;
         if (num < 6)
         {
            extractLocalCopyOfAlphaValues(num);
         }
      };
      imageObj.src = FACE_URLS[num];
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // stopAllMovement
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Stops all cube movement
   //
   //-----------------------------------------------------------------------------------------------

   function stopAllMovement()
   {
      //Rotation throw
      rotThrow.speed = 0;
      //JumpTo
      posJump.active = false;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // get2dDistance
   //
   //Arguments:
   // x0,y0 - start coordinate
   // x1,y1 - destination coordinate
   //
   //Output:
   // void
   //
   //Returns:
   // Distance from two points
   //
   //About:
   // void
   //
   //-----------------------------------------------------------------------------------------------

   function get2dDistance(x0, y0, x1, y1)
   {
      return Math.sqrt(((x0 - x1) * (x0 - x1)) + ((y0 - y1) * (y0 - y1)));
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // areVectorsSimilar
   //
   //Arguments:
   // vec0 - vector 0
   // vec1 - vector 1
   // limit - max acceptable difference between vectors 0 and 1 (float)
   //
   //Output:
   // void
   //
   //Returns:
   // True if vec0 and vec1 are within limit's range
   //
   //About:
   // Many times while dealing with floats our values will be off only slightly, but enough to
   // cause problems. This check helps prevent such problems.
   //
   //-----------------------------------------------------------------------------------------------

   function areVectorsSimilar(vec0, vec1, limit)
   {
      if ((Math.abs(vec0[0] - vec1[0]) < limit) && (Math.abs(vec0[1] - vec1[1]) < limit) &&
          (Math.abs(vec0[2] - vec1[2]) < limit))
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getAngleOfRotation
   //
   //Arguments:
   // vec0 - vector 0
   // vec1 - vector 1
   //
   //Output:
   // void
   //
   //Returns:
   // The angle of rotation in radians between two vectors
   //
   //About:
   // Based on the equation:  cos(@) = (dot(v0,v1) / ||v0||||v1||)
   //
   //-----------------------------------------------------------------------------------------------

   function getAngleOfRotation(vec0, vec1)
   {
      var v3 = twgl.v3;
      var v0 = v3.normalize(vec0);
      var v1 = v3.normalize(vec1);
      return (Math.acos(v3.dot(v0,v1)));
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // dw_getPageOffsets && dw_getScrollOffsets
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // The x,y location of a user click
   //
   //About:
   // Provided by: http://www.dyn-web.com/javascript/element-location/
   // Takes into account scroll bars and other problem-causing offsets
   //
   //-----------------------------------------------------------------------------------------------

   function dw_getPageOffsets(el)
   {
      var sOff = dw_getScrollOffsets(), left = 0, top = 0, props;
      if ( el.getBoundingClientRect )
      {
         props = el.getBoundingClientRect();
         left = props.left + sOff.x;
         top = props.top + sOff.y;
      }
      else // for older browsers
      {
         do
         {
            left += el.offsetLeft;
            top += el.offsetTop;
         }
         while ( (el = el.offsetParent) );
      }

      return { x: Math.round(left), y: Math.round(top) };
   }

   function dw_getScrollOffsets()
   {
      var doc = document, w = window;
      var x, y, docEl;
      if ( typeof w.pageYOffset === 'number' )
      {
         x = w.pageXOffset;
         y = w.pageYOffset;
      }
      else
      {
         docEl = (doc.compatMode && doc.compatMode === 'CSS1Compat')?
         doc.documentElement: doc.body;
         x = docEl.scrollLeft;
         y = docEl.scrollTop;
      }

      return {x:x, y:y};
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getMouseInfo && getTouchInfo
   //
   //Arguments:
   // e - event
   //
   //Output:
   // void
   //
   //Returns:
   // Specific extracted information from an event
   //
   //About:
   // Instead of passing around a large event object, I'd rather extract the information I need
   // and add a little bit of my own information.
   //
   //-----------------------------------------------------------------------------------------------

   function getMouseInfo(e)
   {
      var info = {};
      info.x = e.clientX;
      info.y = e.clientY;
      info.timeStamp = e.timeStamp;
      info.pageX = e.pageX;
      info.pageY = e.pageY;
      info.offsetX = e.offsetX;
      info.offsetY = e.offsetY;
      info.shiftKey = e.shiftKey;
      info.type = "mouse";
      return info;
   }

   function getTouchInfo(e)
   {
      var info = {};
      info.x = e.touches[0].clientX;
      info.y = e.touches[0].clientY;
      info.timeStamp = e.timeStamp;
      info.pageX = e.touches[0].pageX;
      info.pageY = e.touches[0].pageY;
      info.offsetX = info.x;
      info.offsetY = info.y;
      info.shiftKey = false;
      info.type = "touch";
      return info;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // onCubeMouseStartClick && onCubeMouseMove && onCubeMouseEndClick && onCubeMouseScroll
   //
   //Arguments:
   // event object
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // The next three functions are initial handlers for clicking on the cubes canvas. The fourth
   // function handles the mouse scroll events. The initial handler simply gather handlers and then
   // call appropriate handlers.
   //
   //-----------------------------------------------------------------------------------------------

   function onCubeMouseStartClick(e)
   {
      e.preventDefault();
      // Prepare the arguments and call
      var info = getMouseInfo(e);
      onCubeStartClick(info);
   }

   function onCubeMouseMove(e)
   {
      e.preventDefault();
      // Prepare the arguments and call
      var info = getMouseInfo(e);
      onCubeMove(info);
   }

   function onCubeMouseEndClick(e)
   {
      e.preventDefault();
      // Prepare the arguments and call
      var info = getMouseInfo(e);
      onCubeEndClick(info);
   }

   function onCubeMouseScroll(e)
   {
      e.preventDefault();
      var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
      changeZoomByNPercentDo(delta);
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // cube_touchStartHandler && cube_touchMoveHandler && cube_touchEndHandler
   //
   //Arguments:
   // event object
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // The next three functions are initial handlers for touching the cube's canvas. The main test
   // is between 1 or 2 fingers. If 1 finger, it piggy backs on the mouse's click functions.
   //
   //-----------------------------------------------------------------------------------------------

   function cube_touchStartHandler(e)
   {
      e.preventDefault();
      if (e.touches.length >= 2 && !touchInfo.twoFingers) //2 fingers?
      {
         touchInfo.twoFingers = true;
         //Get initial separation
         touchInfo.lastScaled = get2dDistance(e.touches[0].clientX,e.touches[0].clientY,e.touches[1].clientX,e.touches[1].clientY);
         //Back-up current positions of fingers
         touchInfo.history = [];
         touchInfo.history.push({f1x:e.touches[0].clientX,f1y:e.touches[0].clientY,f2x:e.touches[1].clientX,f2y:e.touches[1].clientY});
      }
      else if (e.touches.length == 1) //1 finger?
      {
         //Prepare the arguments and call
         var info = getTouchInfo(e);
         onCubeStartClick(info);
      }
   }

   function cube_touchMoveHandler(e)
   {
      e.preventDefault();
      if (touchInfo.twoFingers)
      {
         //Is this a valid datapoint? or just blood in the fingers?
         var lastH = touchInfo.history[touchInfo.history.length - 1];
         var dist0 = get2dDistance(lastH.f1x,lastH.f1y,e.touches[0].clientX,e.touches[0].clientY);
         var dist1 = get2dDistance(lastH.f2x,lastH.f2y,e.touches[1].clientX,e.touches[1].clientY);

         if (((dist0 >= touchInfo.PULSE_ERROR) && (dist1 >= touchInfo.PULSE_ERROR)) ||
             ((dist0 == 0) && (dist1 >= (touchInfo.PULSE_ERROR * 2.0))) ||
             ((dist1 == 0) && (dist0 >= (touchInfo.PULSE_ERROR * 2.0))))
         {
            touchInfo.history.push({f1x:e.touches[0].clientX,f1y:e.touches[0].clientY,f2x:e.touches[1].clientX,f2y:e.touches[1].clientY});
         }
         else
         {
            //Not a significant enough change... so ignore.
            return;
         }

         //Are we scaling or translating?
         if (!touchInfo.isScaling && !touchInfo.isTranslating)
         {
            //Have we gotten enough samples to determine what to do?
            if (touchInfo.history.length >= 5)
            {
               //If the movements for both fingers are colinear, then we are translating, else, it's a scale
               var finger1 = [e.touches[0].clientX - touchInfo.history[0].f1x, e.touches[0].clientY - touchInfo.history[0].f1y, 0];
               var finger2 = [e.touches[1].clientX - touchInfo.history[0].f2x, e.touches[1].clientY - touchInfo.history[0].f2y, 0];

               //Case 1: 1 Finger not moving, the other is... this is scaling
               if ((Math.abs(finger1[0]) < touchInfo.PULSE_ERROR && Math.abs(finger1[1]) < touchInfo.PULSE_ERROR) ||
                   (Math.abs(finger2[0]) < touchInfo.PULSE_ERROR && Math.abs(finger2[1]) < touchInfo.PULSE_ERROR))
               {
                  touchInfo.isScaling = true;
               }

               //Case 2: Both Fingers are heading in opposite directions... this is scaling
               else if ((finger1[0] > 0 && finger1[1] > 0 && finger2[0] < 0 && finger2[1] < 0) ||
                  (finger1[0] > 0 && finger1[1] < 0 && finger2[0] < 0 && finger2[1] > 0) ||
                  (finger1[0] < 0 && finger1[1] > 0 && finger2[0] > 0 && finger2[1] < 0) ||
                  (finger1[0] < 0 && finger1[1] < 0 && finger2[0] > 0 && finger2[1] > 0))
               {
                  touchInfo.isScaling = true;
               }

               //Case 3: Must be translating...
               else
               {
                  touchInfo.isTranslating = true;
               }
            }
         }

         if (touchInfo.isScaling)
         {
            //Get the distance moved
            var dist = get2dDistance(e.touches[0].clientX,e.touches[0].clientY,e.touches[1].clientX,e.touches[1].clientY);
            //Compare this distance to initial distance
            var delta = (dist - touchInfo.lastScaled)/touchInfo.PINCH_DIVIDER;
            touchInfo.lastScaled = dist;
            changeZoomByNPercentDo(delta);
         }
         else if (touchInfo.isTranslating)
         {
            var lastEvent = touchInfo.history[touchInfo.history.length - 2];

            //Get the avg. vectors of the last event and the current
            var vec1Avg = [(e.touches[0].clientX + e.touches[1].clientX) / 2.0, (e.touches[0].clientY + e.touches[1].clientY) / 2.0];
            var vec0Avg = [(lastEvent.f1x + lastEvent.f2x) / 2.0, (lastEvent.f1y + lastEvent.f2y) / 2.0];

            //Get the equivalent translation in world space
            var zFactor = (ZOOM_HI - (((ZOOM_HI - ZOOM_LO)/100) * zoomPercent) - RADIUS);
            var coords3D = screenCoordto3DCoord(vec1Avg, zFactor);
            var coords3D2 = screenCoordto3DCoord(vec0Avg,zFactor);

            //Translate the cube
            var translation = graphics.m4.identity();
            graphics.m4.translate(translation,[-(coords3D2[0] - coords3D[0]),-(coords3D2[1] - coords3D[1]),0],translation);
            graphics.m4.multiply(matrices.translation,translation,matrices.translation);
         }
      }
      else if (e.touches.length == 1) //1 finger?
      {
         //Prepare the arguments and call
         var info = getTouchInfo(e);
         onCubeMove(info);
      }
   }

   function cube_touchEndHandler(e)
   {
      e.preventDefault();
      if(touchInfo.twoFingers)
      {
         if(e.touches.length == 0)
         {
            touchInfo.twoFingers = false;
            touchInfo.justFinishedTwo = true; //Needed b/c each finger hits pad screen at different time
            touchInfo.isScaling = false;
            touchInfo.isTranslating = false;
         }
      }
      else
      {  // if((e.touches.length < 2) && (!touchInfo.justFinishedTwo)){  //1 finger? (0 is needed for chrome)
         //Prepare the arguments and call
         var info;
         if (e.touches.length == 0)
         {
            info = curserInfo.history[curserInfo.history.length - 1];
         }
         else
         {
            info = getTouchInfo(e);
         }
         onCubeEndClick(info);
      }
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // screenCoordto3DCoord
   //
   //Arguments:
   // x,y - coords in 2d space
   // objDistanct - Distance from viewport to coord in 3d space
   //
   //Output:
   // void
   //
   //Returns:
   // x,y coords in 3d space
   //
   //About:
   // Handles the transition form given x,y screen coords (where 0,0 is the upper left hand of the
   // screen) to viewport coords, which range from -1 to 1 on both the x,y axises. Given a set of
   // screen coords, translates them into 3d space coords
   // Note: ASSUMES that the viewport w,h is the same as our canvas.
   //
   //-----------------------------------------------------------------------------------------------

   function screenCoordto3DCoord(inVec2, objDistance)
   {
      //Get the inverse VP matrix
      var m4 = graphics.m4;
      var VPinverse = m4.identity();
      m4.multiply(matrices.view,matrices.projection,VPinverse);
      m4.inverse(VPinverse,VPinverse);

      //Get the correct viewport coords
      var x = 2.0 * parseFloat(inVec2[0]) / graphics.canvas.width - 1;
      //Notes: 2.0 comes from the range -1 to 1.
      //y is backwards b/c of screen coords opposite scaling.
      var y = -2.0 * parseFloat(inVec2[1]) / graphics.canvas.height + 1;

      //Sadly, twgl doesn't support mat4 * vec4, so make a new mat4
      var pointN = m4.identity();
      pointN[12] = x; pointN[13] = y; pointN[14] = -1;
      var pointF = m4.identity();
      pointF[12] = x; pointF[13] = y; pointF[14] = 1;

      //Multiply!
      m4.multiply(pointN,VPinverse,pointN);
      m4.multiply(pointF,VPinverse,pointF);

      //Normalize
      pointN[15] = 1 / pointN[15];
      pointN[12] = pointN[12] * pointN[15];
      pointN[13] = pointN[13] * pointN[15];
      pointN[14] = pointN[14] * pointN[15];
      pointF[15] = 1 / pointF[15];
      pointF[12] = pointF[12] * pointF[15];
      pointF[13] = pointF[13] * pointF[15];
      pointF[14] = pointF[14] * pointF[15];

      //Scale the objDistance from the NEAR_CLIP to FAR_CLIP
      var scale = (objDistance - NEAR_CLIP) / (FAR_CLIP - NEAR_CLIP);
      scale = Math.min(Math.max(scale, 0), 1);

      //Determine the x,y coords
      var x3d = ((pointF[12] - pointN[12]) * scale) + pointN[12];
      var y3d = ((pointF[13] - pointN[13]) * scale) + pointN[13];

      return [x3d,y3d];
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // coord3DtoScreenCoord
   //
   //Arguments:
   // inVec2 - x,y coords
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // void
   //
   //-----------------------------------------------------------------------------------------------

   function coord3DtoScreenCoord(inVec2)
   {
      //Get the inverse VP matrix
      var m4 = graphics.m4;
      var VP = m4.identity();
      m4.multiply(matrices.view,matrices.projection,VP);

      //Sadly, twgl doesn't support mat4 * vec4, so make a new mat4
      var point = m4.identity();
      point[12] = inVec2[0]; point[13] = inVec2[1];

      //Multiply!
      m4.multiply(point,VP,point);

      //Get the correct viewport coords
      var x = Math.round((( point[12] + 1 ) / 2.0) * graphics.canvas.width );
      //Notes: 2.0 comes from the range -1 to 1.
      //y is backwards b/c of screen coords opposite scaling.
      var y = Math.round((( 1 - point[13]  ) / 2.0) * graphics.canvas.height );

      return [x,y];
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // getTranslateAndRotateScalars
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Based on the current zoom, gets an arbitrary distance from a distance of 1 on screen coords
   // to world coords. Ideally, this would be done on every translation and rotation of the cube,
   // however, this is very expensive.Instead, I'll just sample it on zoom events and window
   // resize events.
   //
   //-----------------------------------------------------------------------------------------------

   function getTranslateAndRotateScalars()
   {
      var zFactor = (ZOOM_HI - (((ZOOM_HI - ZOOM_LO)/100) * zoomPercent) - RADIUS);
      var coords3D = screenCoordto3DCoord([0,0], zFactor);
      var coords3D2 = screenCoordto3DCoord([1,1],zFactor);
      scalarVec3D = [(coords3D2[0] - coords3D[0]),(coords3D2[1] - coords3D[1]),0];
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // onCubeStartClick && onCubeMove && onCubeEndClick
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // The next three functions have been written in such a way to handle both click and touch
   // events. The 'e' passed in is a specific group of variables which contain basic information
   // such as time,x,y,etc.
   //
   //-----------------------------------------------------------------------------------------------

   function onCubeStartClick(e)
   {
      //Turn off initial if on
      graphics.initial = false;
      //Is the resource open?
      if (!curserInfo.isAvailable)
      {
         return;
      }
      //Set a semaphore so only 1 type of input registers at a time
      curserInfo.isAvailable = false;
      curserInfo.isDragging = false;
      posJump.active = false;
      curserInfo.isDown = true;
      if (rotThrow.speed > 0 && (e.type != "touch"))
      {
         curserInfo.isAfterRotate = true;
      }
      else
      {
         curserInfo.isAfterRotate = false;
      }
      rotThrow.speed = 0;
      //Clear the history and push current info to history
      curserInfo.history = [];
      //Push this to the history
      curserInfo.history.push(e);
   }

   function onCubeMove(e)
   {
      if (curserInfo.isDown)
      {
         if (curserInfo.isDragging) //Passed the threshold for drag start?
         {
            //Find distance moved
            var lastX = curserInfo.history[curserInfo.history.length - 1].offsetX;
            var lastY = curserInfo.history[curserInfo.history.length - 1].offsetY;
            //Add to history
            curserInfo.history.push(e);

            var dist = get2dDistance(lastX,lastY,e.offsetX,e.offsetY);
            var distScaled = [scalarVec3D[0] * (e.offsetX - lastX), scalarVec3D[1] * (e.offsetY - lastY)];

            if (movementType.translate || e.shiftKey)
            {
               var translation = graphics.m4.identity();
               graphics.m4.translate(translation,[distScaled[0],distScaled[1], 0],translation);
               graphics.m4.multiply(matrices.translation,translation,matrices.translation);
            }
            else if (movementType.rotate)
            {
               var deltaX = (e.offsetX - lastX);
               var deltaY = (e.offsetY - lastY);
               //Don't rotate if the change of x and y is 0
               if ((deltaX == 0) && (deltaY == 0))
               {
                  return;
               }

               //Drag Rotation
               //d = (theta)r   where r = radius of surrounding sphere and d = arc-length
               var theta = get2dDistance(distScaled[0],distScaled[1],0,0) / SPHERE_RADIUS;

               var rotation = graphics.m4.identity();
               graphics.m4.axisRotate(rotation, [deltaY,deltaX,0], theta, rotation);
               graphics.m4.multiply(matrices.rotation,rotation,matrices.rotation);
            }

         }
         else
         {
            var dist = get2dDistance(curserInfo.history[0].offsetX, curserInfo.history[0].offsetY, e.offsetX, e.offsetY);
            if((curserInfo.history[0].type == "mouse" && dist >= rotDrag.ROTATE_THRESHOLD_CLICK) || (curserInfo.history[0].type == "touch" && dist >= rotDrag.ROTATE_THRESHOLD_TOUCH))
            {
               curserInfo.isDragging = true;
            }
         }
      }
   }

   function onCubeEndClick(e)
   {
      //Only original input device can release mutex
      if (e.type == curserInfo.history[0].type)
      {
         curserInfo.isAvailable = true;
      }
      else
      {
         return;
      }

      if (curserInfo.isDragging)
      {
         if (movementType.rotate && !e.shiftKey) //Rotating throw?
         {
            //Need a minimum of 5 data points
            if (curserInfo.history.length >= 5)
            {
               //Get the info from 5 steps ago, and the current
               var lastE = curserInfo.history[curserInfo.history.length - 5];

               //Get the distance traveled
               var dist = get2dDistance(lastE.offsetX,lastE.offsetY,e.offsetX,e.offsetY);

               if (((e.timeStamp - lastE.timeStamp) <= rotThrow.THROW_THRESHOLD_MS) &&
                   (dist > rotThrow.THROW_THRESHOLD))
               { //Throw threshold?
                  //Direction and speed?
                  var deltaX = (e.offsetX - lastE.offsetX);
                  var deltaY = (e.offsetY - lastE.offsetY);

                  //Illegal throw axis?
                  if ((deltaX == 0) && (deltaY == 0))
                  {
                     rotThrow.speed = 0;
                  }
                  else
                  {
                     rotThrow.axis = [deltaY, deltaX, 0];
                     var delta_t = Math.max(1,(e.timeStamp - lastE.timeStamp));
                     rotThrow.speed = dist/delta_t;
                     //Speed
                     rotThrow.speed = Math.min(rotThrow.speed,10.0);
                     rotThrow.speed = Math.max(rotThrow.speed,1.0);
                     var maxSpeed = rotThrow.MAX_SPEED / Math.max(zoomPercent / rotThrow.SPEED_DIVIDER,1.0);
                     rotThrow.speed = maxSpeed * (rotThrow.speed / 10.0);
                     //Degradation
                     rotThrow.degration = rotThrow.MAX_DEGRATION / Math.max(zoomPercent / rotThrow.DEGRATION_DIVIDER,1.0);
                     //Set deltas
                     rotThrow.delta0 = graphics.time;
                     rotThrow.delta1 = graphics.time;
                  }
               }
            }
         }
      }
      else if(curserInfo.isAfterRotate) //Rotate stop click?
      {
         //Do nothing
      }
      else //Simple Click?
      {
         //Determine where the user clicked...
         if (!selectionMutex)
         {
            return;
         }
         selectionMutex = false;
         var pos = dw_getPageOffsets(graphics.canvas);
         var x = e.pageX - pos.x;
         var y = e.pageY - pos.y;
         selectionInfo.shiftDown = e.shiftKey;
         curserInfo.clickedPos = [x,graphics.canvas.height-y];
         if (renderTextureMutex)
         {
            renderTextureMutex = false;
            graphics.renderToTexture = true;
            selectionInfo.validURL = false;
            render();
            renderTextureMutex = true;
         }
         selectionMutex = true;
      }
      curserInfo.isDown = false;
      curserInfo.isDragging = false;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // transformTo
   //
   //Arguments:
   // faceNum - 0-5
   // x,y - coords
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Sets up the vectors, axis, angles, etc., for the render loop to read in order to jump to a
   // specific location
   //
   //-----------------------------------------------------------------------------------------------

   function transformTo(faceNum, x, y)
   {
      //Turn off initial if on3
      graphics.initial = false;

      //Freeze the Cube
      stopAllMovement();

      //Back-up the input information
      posJump.face = faceNum;
      posJump.x = x;
      posJump.y = y;

      //We need to find the 2 axis of rotation and angles
      var m4 = graphics.m4;
      var v3 = graphics.v3;

      var mMat = m4.identity();
      m4.multiply(mMat,matrices.initialTransforms[faceNum],mMat);
      m4.multiply(mMat,matrices.rotation,mMat);
      var up = v3.normalize([mMat[4],mMat[5],mMat[6]]);

      var upD = [0,0,1];
      //Ensure that the desired vectors are not collinear...
      if (areVectorsSimilar(up,upD,0.0000001))
      {
         posJump.rotation0.angle = 0;
         posJump.rotation0.axis = [0,1,0];
      }
      else
      {
         posJump.rotation0.angle = getAngleOfRotation(up,upD);
         posJump.rotation0.axis = v3.cross(up,upD);
      }

      //Calculate the new matrix
      m4.multiply(mMat,m4.axisRotate(m4.identity(),posJump.rotation0.axis,posJump.rotation0.angle),mMat);
      var right = v3.normalize([mMat[0],mMat[1],mMat[2]]);
      var rightD = [1,0,0];
      //Ensure that the desired vectors are not collinear...
      if (areVectorsSimilar(right,rightD,0.0000001))
      {
         posJump.rotation1.angle = 0;
         posJump.rotation1.axis = [0,1,0];
      }
      else
      {
         posJump.rotation1.angle = getAngleOfRotation(right,rightD);
         posJump.rotation1.axis = v3.cross(right,rightD);
      }

      //Translation to x,y coord
      //Get Cartesian Coords
      var xCoord = (DIAMETER / 2.0) - x;
      var yCoord = (-DIAMETER / 2.0) + y;

      var dist = get2dDistance(matrices.translation[12],matrices.translation[13],xCoord,yCoord);
      var vec = [xCoord - matrices.translation[12], yCoord - matrices.translation[13]];

      //Setup the interval of movement
      //ROTATION
      if (posJump.rotation0.angle == 0)
      {
         posJump.rotation0.updatesLeft = 0;
      }
      else
      {
         posJump.rotation0.updatesLeft = posJump.JUMP_INTERVAL;
         posJump.rotation0.angle /= posJump.rotation0.updatesLeft;
      }
      if (posJump.rotation1.angle == 0)
      {
         posJump.rotation1.updatesLeft = 0;
      }
      else
      {
         posJump.rotation1.updatesLeft = posJump.JUMP_INTERVAL;
         posJump.rotation1.angle /= posJump.rotation1.updatesLeft;
      }
      //TRANSLATION
      if (dist <= 0.001)
      {
         posJump.translate.updatesLeft = 0;
      }
      else
      {
         posJump.translate.updatesLeft = posJump.JUMP_INTERVAL * 2.0;
         posJump.translate.vec = [vec[0]/posJump.translate.updatesLeft,vec[1]/posJump.translate.updatesLeft,0];
      }

      //Already lined up close enough?
      if ((posJump.rotation0.updatesLeft == 0) && (posJump.rotation1.updatesLeft == 0) &&
          (posJump.translate.updatesLeft == 0))
      {
         posJump.active = false;
         posJump.verifyStep = false;
      }

      //Activate the Jump!
      posJump.active = true;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // checkSelectionForValidUrlOrFindValidWithinProximity
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Determines if the selected pixel has a valid URL to go to... furthermore, if the URL is not
   // valid, will attempt to find a pixel within the proximity that is valid.
   //
   //-----------------------------------------------------------------------------------------------

   function checkSelectionForValidUrlOrFindValidWithinProximity()
   {
      var buffer = IMAGE_RGBA[selectionInfo.face];
      var x = selectionInfo.x;
      var y = selectionInfo.y;
      var alpha = buffer[selectionInfo.pixelNum];

      if (!is_UNUSED_ALPHA(alpha))
      {
         //Valid Selection
         selectionInfo.validURL = true;
      }
      else
      {
         //Try to find the closest valid within a range
         selectionInfo.validURL = false;
         for (var i = 1; i <= GUESS_TIERS; i++)
         {
            for (var j = -i; j <= i; j++)
            {
               for (var k = -i; k <= i; k++)
               {
                  var xC = Math.min(Math.max(0, x + j), MAX_XY);
                  var yC = Math.min(Math.max(0, y + k), MAX_XY);
                  var pC = (yC * DIAMETER) + xC;
                  alpha = buffer[pC];
                  if (!is_UNUSED_ALPHA(alpha))
                  {
                     //Valid Selection
                     selectionInfo.x = xC;
                     selectionInfo.y = yC;
                     selectionInfo.pixelNum = pC;
                     selectionInfo.validURL = true;
                     break;
                  }
               }
               if (selectionInfo.validURL)
               {
                  break;
               }
            }
            if (selectionInfo.validURL)
            {
               break;
            }
         }
      }

      selectionInfo.updated = true;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // sampleExposureOfEachCubeFace
   //
   //Arguments:
   // void
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Used to determine which face the user is currently looking at. Does this by getting the MV
   // matrix, and comparing the up vector to the +z axis. The angles are stored in a global
   // variable array.
   //
   //-----------------------------------------------------------------------------------------------

   function sampleExposureOfEachCubeFace()
   {
      var m4 = graphics.m4;
      var v3 = graphics.v3;
      var mMat = m4.identity();
      var mMat2 = m4.identity();
      m4.multiply(mMat,matrices.rotation,mMat);
      m4.multiply(mMat,matrices.translation,mMat);
      m4.multiply(mMat, matrices.view, mMat);

      var exposureSample=[0,0,0,0,0,0];
      for (var i = 0; i < 6; i++)
      {
         m4.multiply(matrices.initialTransforms[i],mMat,mMat2); //MV Matrix
         var u = [0,0,1];
         var v = [mMat2[4],mMat2[5],mMat2[6]];
         var angle = Math.acos(v3.dot(u,v) / (v3.length(u) * v3.length(v)));
         exposureSample[i] = angle;
      }

      return exposureSample;
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // render
   //
   //Arguments:
   // time - automatically inserted.
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Render Loop
   //
   //-----------------------------------------------------------------------------------------------

   function render(time)
   {
      graphics.time = time;
      var m4 = graphics.m4;
      var gl = graphics.gl;

      //Initial page startup
      if (graphics.initial)
      {
         var r = time * .0005;
         m4.multiply(m4.identity(),m4.axisRotate(m4.identity(),[1,0,0],r/2,m4.identity()),matrices.rotation);
         m4.multiply(matrices.rotation,m4.axisRotate(m4.identity(),[0,1,0],r,m4.identity()),matrices.rotation);
      }

      //Are we handling a selection event or taking a sample?
      if (graphics.renderToTexture)
      {
         gl.bindFramebuffer(gl.FRAMEBUFFER,graphics.texFB);
      }

      //Clear the frame buffers
      gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

      //Thrown rotation?
      if (rotThrow.speed > 0)
      {
         rotThrow.delta1 = time;
         if (rotThrow.delta1 - rotThrow.delta0 >= 16.0)
         {
            rotThrow.delta0 = rotThrow.delta1 - 16.0;
            m4.multiply(matrices.rotation,m4.axisRotate(m4.identity(),rotThrow.axis,rotThrow.speed,m4.identity()),matrices.rotation);
            rotThrow.speed = Math.max(0,rotThrow.speed - rotThrow.degration);
         }
      }

      //FaceJump?
      if (posJump.active)
      {
         if (posJump.translate.updatesLeft > 0 || posJump.rotation0.updatesLeft > 0 || posJump.rotation1.updatesLeft > 0)
         {
            if (posJump.translate.updatesLeft > 0)
            {
               m4.multiply(matrices.translation,m4.translate(m4.identity(),posJump.translate.vec,m4.identity()),matrices.translation);
               posJump.translate.updatesLeft -= 1;
            }
            if (posJump.rotation0.updatesLeft > 0)
            {
               m4.multiply(matrices.rotation,m4.axisRotate(m4.identity(),posJump.rotation0.axis,posJump.rotation0.angle,m4.identity()),matrices.rotation);
               posJump.rotation0.updatesLeft -= 1;
            }
            else if(posJump.rotation1.updatesLeft > 0)
            {
               m4.multiply(matrices.rotation,m4.axisRotate(m4.identity(),posJump.rotation1.axis,posJump.rotation1.angle,m4.identity()),matrices.rotation);
               posJump.rotation1.updatesLeft -= 1;
            }
         }
         else if (!posJump.verifyStep)
         {
            posJump.verifyStep = true;
            transformTo(posJump.face,posJump.x,posJump.y);
         }
         else
         {
            posJump.active = false;
            posJump.verifyStep = false;
         }
      }

      //For each side of the cube, transform!
      var mMat = m4.identity();
      m4.multiply(mMat,matrices.rotation,mMat);
      m4.multiply(mMat,matrices.translation,mMat);
      m4.multiply(mMat, matrices.view, mMat);
      m4.multiply(mMat, matrices.projection, mMat);

      for (var i = 0; i < 6; i++)
      {
         var uni = graphics.drawObjects[i].uniforms;

         m4.multiply(matrices.initialTransforms[i],mMat,uni.u_worldViewProjection);

         if (graphics.renderToTexture)
         {
            uni.u_mousePosition = [curserInfo.clickedPos[0],curserInfo.clickedPos[1],ALPHAS[i][0],1.0];
         }
         else
         {
            uni.u_mousePosition[3] = 0.0;
         }
      }

      //Draw!
      twgl.drawObjectList(gl,graphics.drawObjects);

      //If we rendered to texture, extract the selection information
      if (graphics.renderToTexture)
      {
         var data = new Uint8Array(graphics.canvas.width * graphics.canvas.height * 4);
         gl.readPixels(0, 0, graphics.canvas.width, graphics.canvas.height, gl.RGBA, gl.UNSIGNED_BYTE, data);
         //Find out where the user clicked based off pixel alpha value
         var face = -1;
         for (var i = 0; i < data.length; i += 4)
         {
            if ((data[i+3] != 0) && (data[i+3] <= ALPHAS[5][1] + 5))
            {
               for (var j=0; j < 6; j++)
               {
                  if (Math.abs(data[i+3] - ALPHAS[j][1]) <= 1)
                  {
                     face = j;
                     break;
                  }
               }

               if (face == -1) { continue; } //Illegal image file...

               //Interpret the response
               var pixelX = Math.min((data[i] << 4) | (data[i+1] >> 4),MAX_XY);
               var pixelY = Math.min((data[i+2] << 4) | (data[i+1] % 16),MAX_XY);
               var pixelNum = pixelY * DIAMETER + pixelX;
               selectionInfo.x = pixelX;
               selectionInfo.y = pixelY;
               selectionInfo.face = face;
               selectionInfo.faceName = FACE_NAMES[face];
               selectionInfo.pixelNum = pixelNum;
               checkSelectionForValidUrlOrFindValidWithinProximity();
               break;
            }
         }
         gl.bindFramebuffer(gl.FRAMEBUFFER,null);
         graphics.renderToTexture = false;
         return;
      }

      //Request the next call to this function
      requestAnimationFrame(render);
   }

   //-----------------------------------------------------------------------------------------------
   //Name:
   // init
   //
   //Arguments:
   // canvasID - Canvas where the cube will be initialized upon
   //
   //Output:
   // void
   //
   //Returns:
   // void
   //
   //About:
   // Initializes webGL, the cube, handles, etc.
   //
   //-----------------------------------------------------------------------------------------------

   function init(canvasID)
   {
      //Load all the images into RGBA arrays
      extractLocalCopyOfAlphaValues(0);

      //Initialize webGL
      graphics.canvas = canvasID;
      graphics.gl = twgl.getWebGLContext(graphics.canvas);

      var gl = graphics.gl;
      var m4 = graphics.m4;

      //Initialize global vars
      matrices.view = m4.identity();
      matrices.projection = m4.identity();
      matrices.rotation = m4.identity();
      matrices.translation = m4.identity();
      matrices.initialTransforms = [m4.identity(),m4.identity(),m4.identity(),m4.identity(),m4.identity(),m4.identity()];

      //Initialize the webGL Cube
      twgl.setAttributePrefix("a_");
      gl.enable(gl.DEPTH_TEST);

      //Instantiate the shaders
      //Anti-aliasing functions : https://github.com/mattdesl/glsl-fxaa
      var shader = twgl.createProgramInfo(gl, ["                                                    \
         uniform mat4 u_worldViewProjection;                                                        \
         attribute vec4 a_position;                                                                 \
         attribute vec2 a_texcoord;                                                                 \
         varying vec2 v_texCoord;                                                                   \
                                                                                                    \
         void main(){                                                                               \
            v_texCoord = a_texcoord;                                                                \
                                                                                                    \
            gl_Position = u_worldViewProjection * a_position;                                       \
         }"
         ,
         "precision mediump float;                                                                  \
         varying vec2 v_texCoord;                                                                   \
         uniform sampler2D u_diffuse;                                                               \
         uniform vec4 u_mousePosition;                                                              \
         uniform float u_diameter;                                                                  \
                                                                                                    \
         void main(){                                                                               \
            /*This is JFowlers Magic Function for block selection identification*/                  \
            if((u_mousePosition.w == 1.0) &&                                                        \
               (abs(gl_FragCoord.y - u_mousePosition.y) <= 0.5) &&                                  \
               (abs(gl_FragCoord.x - u_mousePosition.x) <= 0.5))                                    \
            {                                                                                       \
               float pixelX=floor(v_texCoord.x*(u_diameter-1.0)+0.5);                               \
               float pixelY=floor(v_texCoord.y*(u_diameter-1.0)+0.5);                               \
               float a=floor(pixelX/16.0);                                                          \
               float b=floor((floor(mod(pixelX,16.0))*16.0)+floor(mod(pixelY,16.0)));               \
               float c=floor(pixelY/16.0);                                                          \
               gl_FragColor=vec4(float(a)/255.0,float(b)/255.0,float(c)/255.0,u_mousePosition.z);   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
               gl_FragColor = texture2D(u_diffuse, v_texCoord);                                     \
            }                                                                                       \
         }                                                                                          \
      "]);

      //Load the textures
      var textures = twgl.createTextures(gl,{
         f0: {src: FACE_URLS[0]},
         f1: {src: FACE_URLS[1]},
         f2: {src: FACE_URLS[2]},
         f3: {src: FACE_URLS[3]},
         f4: {src: FACE_URLS[4]},
         f5: {src: FACE_URLS[5]}});

      //Set initial transforms to set up box
      //Create a single 2d-plane
      var plane = twgl.primitives.createPlaneBufferInfo(gl, DIAMETER, DIAMETER);
      //Front
      m4.identity(matrices.initialTransforms[0]);
      m4.translate(matrices.initialTransforms[0], [0,0,RADIUS], matrices.initialTransforms[0]);
      m4.rotateX(matrices.initialTransforms[0], Math.PI / 2, matrices.initialTransforms[0]);
      //Right
      m4.identity(matrices.initialTransforms[1]);
      m4.translate(matrices.initialTransforms[1], [RADIUS,0,0], matrices.initialTransforms[1]);
      m4.rotateY(matrices.initialTransforms[1], Math.PI / 2, matrices.initialTransforms[1]);
      m4.rotateX(matrices.initialTransforms[1], Math.PI / 2, matrices.initialTransforms[1]);
      //Back
      m4.identity(matrices.initialTransforms[2]);
      m4.translate(matrices.initialTransforms[2], [0,0,-RADIUS], matrices.initialTransforms[2]);
      m4.rotateY(matrices.initialTransforms[2], Math.PI, matrices.initialTransforms[2]);
      m4.rotateX(matrices.initialTransforms[2], Math.PI / 2, matrices.initialTransforms[2]);
      //Left
      m4.identity(matrices.initialTransforms[3]);
      m4.translate(matrices.initialTransforms[3], [-RADIUS,0,0], matrices.initialTransforms[3]);
      m4.rotateY(matrices.initialTransforms[3], Math.PI * 1.5, matrices.initialTransforms[3]);
      m4.rotateX(matrices.initialTransforms[3], Math.PI / 2, matrices.initialTransforms[3]);
      //Top
      m4.identity(matrices.initialTransforms[4]);
      m4.translate(matrices.initialTransforms[4], [0,RADIUS,0], matrices.initialTransforms[4]);
      //Bottom
      m4.identity(matrices.initialTransforms[5]);
      m4.translate(matrices.initialTransforms[5], [0,-RADIUS,0], matrices.initialTransforms[5]);
      m4.rotateX(matrices.initialTransforms[5], Math.PI, matrices.initialTransforms[5]);
      //Collection of uniforms
      var uniforms = [];
      uniforms.push(
         {u_diffuse: textures.f0, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER},
         {u_diffuse: textures.f1, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER},
         {u_diffuse: textures.f2, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER},
         {u_diffuse: textures.f3, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER},
         {u_diffuse: textures.f4, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER},
         {u_diffuse: textures.f5, u_worldViewProjection: m4.identity(), u_mousePosition: [0,0,0,0], u_diameter: DIAMETER});

      //Collection of objects to be drawn
      graphics.drawObjects.push(
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[0]},
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[1]},
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[2]},
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[3]},
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[4]},
         {programInfo: shader,bufferInfo: plane,uniforms: uniforms[5]});

      //Set up texture-rendering framebuffer with both a color texture buffer and depth buffer
      graphics.texFB = gl.createFramebuffer(gl.FRAMEBUFFER);
      gl.bindFramebuffer(gl.FRAMEBUFFER,graphics.texFB);
      graphics.texFB.width = 1400; //2048;
      graphics.texFB.height = 1400; //2048;
      graphics.tex = gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D,graphics.tex);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, graphics.texFB.width, graphics.texFB.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
      var renderbuffer = gl.createRenderbuffer();
      gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
      gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, graphics.texFB.width, graphics.texFB.height);

      gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, graphics.tex, 0);
      gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, renderbuffer);

      gl.bindTexture(gl.TEXTURE_2D, null);
      gl.bindRenderbuffer(gl.RENDERBUFFER, null);
      gl.bindFramebuffer(gl.FRAMEBUFFER, null);

      //Initialize view and projection matrices
      var zoom = ZOOM_HI - (((ZOOM_HI - ZOOM_LO)/100) * zoomPercent);
      m4.lookAt([0,0,zoom], [0,0,0], [0,1,0], matrices.view);
      m4.inverse(matrices.view, matrices.view);
      twgl.resizeCanvasToDisplaySize(gl.canvas);
      gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
      matrices.projection = m4.perspective(FOV, gl.canvas.width / Math.max(gl.canvas.height,0.0001),  NEAR_CLIP, FAR_CLIP);
      //Get scalar for rotate and translate
      getTranslateAndRotateScalars();

      //Add a window resize event
      window.addEventListener("resize", function()
      {
         //Update window adjustment
         twgl.resizeCanvasToDisplaySize(gl.canvas);
         gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
         matrices.projection = m4.perspective(FOV, gl.canvas.width / Math.max(gl.canvas.height,0.0001), NEAR_CLIP, FAR_CLIP);
         //Update scalar multiplier for rotate and translate
         getTranslateAndRotateScalars();
      });

      var element = graphics.canvas;
      //Mouse Scroll Event
      if (element.addEventListener)
      {
         element.addEventListener("mousewheel", onCubeMouseScroll, false); // IE9, Chrome, Safari, Opera
         element.addEventListener("DOMMouseScroll", onCubeMouseScroll, false); // Firefox
      }
      else
      {
         element.attachEvent("onmousewheel", onCubeMouseScroll); // IE 6/7/8
      }

      //Mouse and touch screen, cube manipulation
      $(element).mousedown(onCubeMouseStartClick);
      $(element).mousemove(onCubeMouseMove);
      $(element).mouseup(onCubeMouseEndClick);
      element.addEventListener("touchstart", cube_touchStartHandler, false);
      element.addEventListener("touchmove", cube_touchMoveHandler, false);
      element.addEventListener("touchend", cube_touchEndHandler, false);

      //Begin animation loop
      requestAnimationFrame(render);
   }

   //Set it up

   PIX_PER_BLOCK_LENGTH = parseInt(pixelsPerBlock);
   BLOCKS_PER_FACE_LENGTH = parseInt(blocksPerFaceAcross);
   FACE_URLS = faceImgPaths;
   FACE_NAMES = faceImgNames;

   init(canvasID);
}
