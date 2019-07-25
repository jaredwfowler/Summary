<!DOCTYPE html>

<?php
include_once public_path('meta.php');
?>

<html lang="en" id="html"><head id="head">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="csrf-token" content="{{ csrf_token() }}" />
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <meta name="keywords" content=" <?php echo $META_PHP['WEBSITE_KEYWORDS']['value']; ?>">
    <meta name="description" content=" <?php echo $META_PHP['WEBSITE_DESCRIPTION']['value']; ?>">

    <!--Import Google Icon Font-->
    <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
    <!--Import materialize.css-->
    <link type="text/css" rel="stylesheet" href="css/materialize.min.css"  media="screen,projection"/>

    @if (null == isset($page_mode))

    <!--Import Custom Styles.css-->
    <link rel="stylesheet" href="{{asset('css/styles.css')}}">

    @endif

    <script src="{{asset('js/jquery-3.2.1.min.js')}}"></script>
    <script src="{{asset('js/materialize.min.js')}}"></script>
    <script> $.ajaxSetup({ headers: {'X-CSRF-TOKEN': $('meta[name="csrf-token"]').attr('content')} }); </script>

    <script> var META = <?php echo $META_JS; ?>; var publicURL = "<?php echo url('/'); ?>";</script>
    <script src="{{asset('js/tuxedocat.js')}}"></script>
    <script> var tuxedocatBuilder = new TuxedoCat_BobTheBuilder(META, publicURL);</script>

    <title>
        <?php echo $META_PHP['WEBSITE_TITLE']['value']; ?> -
        @yield('title')
    </title>

</head><body id="body"><header id="header">

    @if (null == isset($page_mode))

    @include('modules.header')

    @endif

    @include('modules.contactUs_modal')

    <div id="navContainer">
        <div class="row">
            <div class="col s12 m12">
                @include('modules.navbar')
            </div>
        </div>
    </div>

</header><main id="main">

    <br>
    
    <div id="contentContainer">
        <div class="row">
            <div class="col s12 m12" id="pageContent">
                @yield('content')
            </div>
        </div>
    </div>

    @if (null == isset($page_mode))

    <p id="keywords_addition" style="display:none;"> <?php echo $META_PHP['WEBSITE_KEYWORDS']['value']; ?></p>

    @endif

</main><footer id="footer">

    @if (null == isset($page_mode))

    @include('modules.footer')

    @endif

</footer>

    <script>
    
    @if (null == isset($page_mode))

    $(document).ready(function() 
    {
        var parentID2 = document.getElementById("body");
        tuxedocatBuilder.Build_Object("WEBSITE_BACKGROUND_IMAGE", parentID2);
    });

    @endif

    </script>

    @yield('js')

</body></html>

