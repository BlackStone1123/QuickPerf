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

        function drawBackground(ctx) {
            ctx.save();
            ctx.fillStyle = "#ffffff";
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.strokeStyle = "#d7d7d7";
            ctx.beginPath();
            // Horizontal grid lines
            for (var i = 0; i < 12; i++) {
                ctx.moveTo(0, canvas.yGridOffset + i * canvas.yGridStep);
                ctx.lineTo(canvas.width, canvas.yGridOffset + i * canvas.yGridStep);
            }

            // Vertical grid lines
            var height = 35 * canvas.height / 36;
            var yOffset = canvas.height - height;
            var xOffset = 0;
            for (i = 0; i < chart.gridSize; i++) {
                ctx.moveTo(xOffset + i * chart.gridStep, yOffset);
                ctx.lineTo(xOffset + i * chart.gridStep, height);
            }
            ctx.stroke();

            // Right ticks
            ctx.strokeStyle = "#666666";
            ctx.beginPath();
            var xStart = canvas.width - tickMargin;
            ctx.moveTo(xStart, 0);
            ctx.lineTo(xStart, canvas.height);
            for (i = 0; i < 12; i++) {
                ctx.moveTo(xStart, canvas.yGridOffset + i * canvas.yGridStep);
                ctx.lineTo(canvas.width, canvas.yGridOffset + i * canvas.yGridStep);
            }
            ctx.moveTo(0, canvas.yGridOffset + 9 * canvas.yGridStep);
            ctx.lineTo(canvas.width, canvas.yGridOffset + 9 * canvas.yGridStep);
            ctx.closePath();
            ctx.stroke();

            ctx.restore();
        }

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

//                if (i === 0) {
//                    ctx.moveTo(x, y);
//                } else {
//                    ctx.lineTo(x, y);
//                }
                ctx.fillRect(x, y, stride, root.height - y)
            }
            //ctx.stroke();
            ctx.restore();
        }

        onPaint: {
            var ctx = canvas.getContext("2d");
            ctx.globalCompositeOperation = "source-over";

            // drawBackground(ctx);

            var points = [];

            if(startPos >= pointSetModel.length)
            {
                return;
            }

            for (var i = startPos, j = 0; i< startPos + numPoints ; i++, j++) {

                points.push({x: j * stride, y: root.height - pointSetModel[i]});
            }

            drawLines(ctx, lineColor, points);
        }
    }
}
