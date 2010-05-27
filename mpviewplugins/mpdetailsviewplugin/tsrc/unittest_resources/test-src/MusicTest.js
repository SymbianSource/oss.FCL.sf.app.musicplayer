MusicTest = TestCase('MusicTest');

MusicTest.prototype.testMusicContext = function() {
    var context = music.privateFunctions.getMusicContext();
	assertTrue(context.object.data.miniview.indexOf("London Calling") != -1);
	assertTrue(context.object.data.miniview.indexOf("The Clash") != -1);
};

MusicTest.prototype.testNoa = function() {
    var context = music.privateFunctions.getNoa();
    assertTrue(context.username.indexOf("lcoubert10") != -1);
    assertTrue(context.password.indexOf("lcoubert01") != -1);
};

MusicTest.prototype.testInitPlayer = function() {
//    document.createElement('div').setAttribute("id", "ovi.player.share.ui");
    /*:DOC += <div id="ovi.player.share.ui"></div>*/
    music.initPlayer();
	var ui = document.getElementById("ovi.player.share.ui");
    assertTrue(ui.innerHTML.indexOf('player') >= 0);
};

MusicTest.prototype.setUp = function() {
    window.context = {
        username: function(){
            return "lcoubert10"
        },
        password: function(){
            return "lcoubert01"
        },
        title: function(){
            return "London Calling"
        },
        album: function(){
            return "dummyuri"
        },
        artist: function(){
            return "The Clash"
        },
        link: function(){
            return "http://music.ovi.com/ru/ru/pc/Product/Queen/Bohemian-Rhapsody/8019069"
        },
        albumArtBase64: function(){return 0;}
    }
};

MusicTest.prototype.tearDown = function() { };


