class LightTime {
  constructor(h, m) {
    this.h = h;
    this.m = m;
  }
}

const lightTimes = [
  new LightTime(7, 0),
  new LightTime(13, 30),
  new LightTime(13, 35),
  new LightTime(21, 00),
];

// for (let index = lightTimes.length - 1; index >= 0; index--) {
//   const timeNow = new Date();
//   const lightSaved = lightTimes[index];
//   console.log(timeNow.getHours(), timeNow.getMinutes());
//   if (
//     lightSaved.h <= timeNow.getHours() &&
//     lightSaved.m <= timeNow.getMinutes()
//   ) {
//     console.log("ativando horario", lightSaved.h, lightSaved.m);
//     break;
//   }
// }

let index = lightTimes.length - 1;

while (index--) {
  const timeNow = new Date();
  const lightSaved = lightTimes[index];
  console.log(timeNow.getHours(), timeNow.getMinutes());
  if (
    lightSaved.h <= timeNow.getHours() &&
    lightSaved.m <= timeNow.getMinutes()
  ) {
    console.log("ativando horario", lightSaved.h, lightSaved.m);
    break;
  }
}
