const fs = require('fs');
const zlib = require('zlib');
var zopfli = require('node-zopfli');
const { gzip } = require('@gfx/zopfli');

function getByteArray(file) {
  let fileData = file.toString('hex');
  let result = [];
  for (let i = 0; i < fileData.length; i += 2)
    result.push('0x' + fileData[i] + '' + fileData[i + 1]);
  return result;
}


const options = {
  verbose: false,
  verbose_more: false,
  numiterations: 15,
  blocksplitting: true,
  blocksplittinglast: false,
  blocksplittingmax: 15
};

let js = fs.readFileSync(__dirname + '/dist/js/app.js');
let html = `
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width,initial-scale=1.0">
    <meta name="referrer" content="no-referrer">
    <link rel=icon href=/favicon.ico>
    <title>LITOS User Interface</title>
  </head>
  <body>
    <noscript>
      <strong>We're sorry but the LITOS User Interface  doesn't work properly without JavaScript enabled. Please enable it to continue.</strong>
    </noscript>
    <div id="app"></div>
    <script>${js}</script>
  </body>
</html>
`;
/*
// Gzip the index.html file with JS Code.
var input = new Buffer(html);
var gziped = zopfli.gzipSync(input, options);
fs.writeFileSync(__dirname + '/dist/index.html.gz', gziped, 'utf8');


// start versuch progmem
const gzippedIndex = zlib.gzipSync(html, { 'level': zlib.constants.Z_BEST_COMPRESSION });
let indexHTML = getByteArray(gzippedIndex);

let source =
  `
const uint32_t DASH_HTML_SIZE = ${indexHTML.length};
const uint8_t DASH_HTML[] PROGMEM = { ${indexHTML} };


fs.writeFileSync(__dirname + '/dist/webpage.h', source, 'utf8');`;
*/

// Produce a second variant with zopfli
// Zopfli is a improved zip algorithm by google
// Takes much more time and maybe is not available on every machine
input = new Buffer(html);;
gzip(input, { numiterations: 15 }, (err, output) => {
  indexHTML = output;
  let source =    `const uint32_t webpage_size = ${indexHTML.length};
const uint8_t webpage[] PROGMEM = { ${indexHTML} };
`;

  fs.writeFileSync(__dirname + '/dist/webpage.h', source, 'utf8');
});


gzip(fs.readFileSync(__dirname + '/dist/favicon.ico'), { numiterations: 15 }, (err, output) => {
  binary = output;
  let source =    `const uint32_t favicon_size = ${binary.length};
const uint8_t favicon[] PROGMEM = { ${binary} };
`;
  fs.writeFileSync(__dirname + '/dist/favicon.h', source, 'utf8');
});


gzip(fs.readFileSync(__dirname + '/dist/fonts/element-icons.ttf'), { numiterations: 15 }, (err, output) => {
  binary = output;
  let source =    `const uint32_t element_icons_size = ${binary.length};
const uint8_t element_icons[] PROGMEM = { ${binary} };
`;
  fs.writeFileSync(__dirname + '/dist/element_icons.h', source, 'utf8');
});

// ende progmem



/*
fs.createReadStream(__dirname + '/dist/favicon.ico')
  .pipe(zopfli.createGzip(options))
  .pipe(fs.createWriteStream(__dirname + '/dist/favicon.ico.gz'));
fs.unlinkSync(__dirname + '/dist/favicon.ico');
fs.unlinkSync(__dirname + '/dist/index.html');
fs.unlinkSync(__dirname + '/dist/fonts/element-icons.ttf');
*/