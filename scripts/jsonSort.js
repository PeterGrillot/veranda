const fs = require('fs');

const FILE_NAME = process.argv[2];
const SORT_BY = process.argv[3];
const PATH = `./${FILE_NAME}.json`;
const FILE_TYPE = 'utf8';

function sortBy (key, cb) {
  if (!cb) cb = () => 0;
  return (a, b) => (a[key] > b[key]) ? 1
    : ((b[key] > a[key]) ? -1 : cb(a, b));
}

fs.readFile(PATH, FILE_TYPE, (rErr, data) => {
  if (rErr) {
    console.log(`Error reading file from disk: ${rErr}`);
  } else {

    // parse JSON string to JSON object
    const parsed = JSON.parse(data);
    let keyToSortBy = 'title';
    if (SORT_BY) {
      keyToSortBy = SORT_BY;
    }
    const sorted = parsed.library.sort(sortBy([keyToSortBy]));
    const jsonString = JSON.stringify({cmd: parsed.cmd, library: sorted});
    
    fs.writeFile(PATH, jsonString, (wErr) => {
      if(wErr) {
        console.log(`Error writing file from disk: ${wErr}`);
      } else {
        console.log('Complete!')
      }
    });
  }
});