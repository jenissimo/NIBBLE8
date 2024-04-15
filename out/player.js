var Module = {
    arguments: [
        '--cart', 'demos/synthwave.n8',
        '--width', '480',
        '--height', '360',
        '--player', '1',
    ],
    canvas: function () {
        var canvas = document.getElementById('canvas');
        canvas.webkitRequestFullScreen(Element.ALLOW_KEYBOARD_INPUT);
        return canvas;
    }()
}