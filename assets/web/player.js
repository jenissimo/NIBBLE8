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

document.getElementById('fullscreenBtn').addEventListener('click', function () {
    var canvas = document.getElementById('canvas');
    if (!document.fullscreenElement) {
        if (canvas.requestFullscreen) {
            canvas.requestFullscreen(); // Standard
        } else if (canvas.webkitRequestFullscreen) {
            canvas.webkitRequestFullscreen(Element.ALLOW_KEYBOARD_INPUT); // Chrome, Safari (legacy)
        } else if (canvas.msRequestFullscreen) {
            canvas.msRequestFullscreen(); // IE11
        }
    } else {
        if (document.exitFullscreen) {
            document.exitFullscreen(); // Standard
        } else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen(); // Chrome, Safari (legacy)
        } else if (document.msExitFullscreen) {
            document.msExitFullscreen(); // IE11
        }
    }
});

// Optional: Automatically request fullscreen on canvas click (can remove if not needed)
canvas.addEventListener('click', function () {
    if (!document.fullscreenElement) {
        canvas.requestFullscreen();
    }
});