import QtQuick 2.0

Item {
    id: root

    property var pointSetModel: null
    property real stride: 1.0
    property var lineColor: "red"
    property int startPos: 0
    property int numPoints: 0

    function repaint()
    {
        if(canvas.available)
        {
            var ctx = canvas.getContext("2d");
            ctx.reset();
            canvas.requestPaint();
        }
    }

    onStartPosChanged: {
        repaint();
    }

    onNumPointsChanged: {
        repaint();
    }

    Canvas {
        id: canvas

        anchors.fill: parent
        //antialiasing: true

        // Uncomment below lines to use OpenGL hardware accelerated rendering.
        // See Canvas documentation for available options.
//                 renderTarget: Canvas.FramebufferObject
//                 renderStrategy: Canvas.Threaded

        function drawLines(ctx, color, points)
        {
            ctx.save();
            ctx.strokeStyle = color;
            ctx.fillStyle = color;
            ctx.lineWidth = numPoints > 500 ? 1 : 2

            ctx.beginPath();

            var end = points.length;

            for (var i = 0; i < end; i++) {
                var x = points[i].x;
                var y = points[i].y;

                if(root.height - y > 1)
                {
                    ctx.fillRect(x, y, stride, root.height - y);
                }
            }
            ctx.restore();
        }

        onPaint: {
            var ctx = canvas.getContext("2d");
            ctx.globalCompositeOperation = "source-over";

            var points = [];

            if(startPos >= pointSetModel.length)
            {
                return;
            }

            for (var i = startPos, j = 0; i< startPos + numPoints ; i++, j++) {

                points.push({x: j * stride, y: root.height - pointSetModel[i] * root.height / 100 });
            }

            drawLines(ctx, lineColor, points);
        }
    }
}
