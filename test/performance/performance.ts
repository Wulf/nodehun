import { Nodehun } from 'nodehun'

const fs = require('fs')
const path = require('path')
// @ts-ignore
const { performance } = require('perf_hooks')
const ChartjsNode = require('chartjs-node');

const Nodehun3: typeof Nodehun = require('bindings')('Nodehun')
const Nodehun2 = require('nodehun')

/////////////////////////////////////////////////////
// CONFIGURATION
/////////////////////////////////////////////////////
const samples_spell = 10
const iterations_spell = 100
const samples_suggest = 10
const iterations_suggest = 100
const correct = 'color'
const incorrect = 'colour'
const dictionaryType = 'en_US'
const d = {
  affix: fs.readFileSync(path.resolve(__dirname, '../dictionaries/en_us.aff')),
  dictionary: fs.readFileSync(path.resolve(__dirname, '../dictionaries/en_us.dic'))
}
/////////////////////////////////////////////////////

interface Nodehun2 {
  isCorrect: Function
  isCorrectSync: Function
  spellSuggestions: Function
  spellSuggestionsSync: Function
}

const nodehun3: Nodehun = new Nodehun3(d.affix, d.dictionary)
const nodehun2: Nodehun2 = new Nodehun2(d.affix, d.dictionary)

const suggestSync3Test = (word: string): number => {
  let start = performance.now()
  nodehun3.suggestSync(word)
  let end = performance.now()
  return end - start
}

const suggestSync2Test = (word: string): number => {
  let start = performance.now()
  nodehun2.spellSuggestionsSync(word)
  let end = performance.now()
  return end - start
}

const suggest3Test = async (word: string) => {
  let start = performance.now()
  await nodehun3.suggest(word)
  let end = performance.now()
  return end - start
};

const suggest2Test = async (word: string) => {
  let start = performance.now()
  await new Promise(resolve => nodehun2.spellSuggestions(word, resolve))
  let end = performance.now()
  return end - start
}

const spellSync3Test = (word: string): number => {
  let start = performance.now()
  nodehun3.spellSync(word)
  let end = performance.now()
  return end - start
}

const spellSync2Test = (word: string): number => {
  let start = performance.now()
  nodehun2.isCorrectSync(word)
  let end = performance.now()
  return end - start
}

const spell3Test = async (word: string) => {
  let start = performance.now()
  await nodehun3.spell(word)
  let end = performance.now()
  return end - start
};

const spell2Test = async (word: string) => {
  let start = performance.now()
  await new Promise(resolve => nodehun2.isCorrect(word, resolve))
  let end = performance.now()
  return end - start
}

const suggestSyncTest = (iterations: number) => {
  let time2_correct = 0
    , time3_correct = 0
    , time2_incorrect = 0
    , time3_incorrect = 0

  for (let i = 0; i < iterations; i++) {
    time2_correct += suggestSync2Test(correct)
    time2_incorrect += suggestSync2Test(incorrect)
    time3_correct += suggestSync3Test(correct)
    time3_incorrect += suggestSync3Test(incorrect)
  }

  return {
    time2_correct,
    time2_incorrect,
    time3_correct,
    time3_incorrect
  }
}

const suggestTest = async (iterations: number) => {
  let time2_correct = 0
    , time3_correct = 0
    , time2_incorrect = 0
    , time3_incorrect = 0

  for (let i = 0; i < iterations; i++) {
    time2_correct += await suggest2Test(correct)
    time2_incorrect += await suggest2Test(incorrect)
    time3_correct += await suggest3Test(correct)
    time3_incorrect += await suggest3Test(incorrect)
  }

  return {
    time2_correct,
    time2_incorrect,
    time3_correct,
    time3_incorrect
  }
}



const spellSyncTest = (iterations: number) => {
  let time2_correct = 0
    , time3_correct = 0
    , time2_incorrect = 0
    , time3_incorrect = 0

  for (let i = 0; i < iterations; i++) {
    time2_correct += spellSync2Test(correct)
    time2_incorrect += spellSync2Test(incorrect)
    time3_correct += spellSync3Test(correct)
    time3_incorrect += spellSync3Test(incorrect)
  }

  return {
    time2_correct,
    time2_incorrect,
    time3_correct,
    time3_incorrect
  }
}

const spellTest = async (iterations: number) => {
  let time2_correct = 0
    , time3_correct = 0
    , time2_incorrect = 0
    , time3_incorrect = 0

  for (let i = 0; i < iterations; i++) {
    time2_correct += await spell2Test(correct)
    time2_incorrect += await spell2Test(incorrect)
    time3_correct += await spell3Test(correct)
    time3_incorrect += await spell3Test(incorrect)
  }

  return {
    time2_correct,
    time2_incorrect,
    time3_correct,
    time3_incorrect
  }
}

const standardDeviation = (values: number[]) => {
  var avg = average(values)

  var squareDiffs = values.map(function (value) {
    var diff = value - avg
    var sqrDiff = diff * diff
    return sqrDiff
  })

  var avgSquareDiff = average(squareDiffs)

  var stdDev = Math.sqrt(avgSquareDiff)
  return stdDev
}

const average = (data: number[]) => {
  var sum = data.reduce(function (sum, value) {
    return sum + value
  }, 0)

  var avg = sum / data.length
  return avg
}

/**
 * Takes an array of objects and returns the average and standard deviation
 * for each numeric key
 * @param results 
 */
const analyzeResults = (results: Array<Record<string, number>>): Record<string, { average: number, standardDeviation: number }> => {
  if (!results || results.length == 0) throw 'empty results'

  const obj = results[0]
  const analysis: Record<string, { average: number, standardDeviation: number }> = {}

  Object.keys(obj).forEach(key => {
    if (typeof obj[key] == 'number') {
      const data = results.map(r => r[key])
      const avg = average(data)
      const std = standardDeviation(data)
      analysis[key] = {
        average: avg,
        standardDeviation: std
      }
    }
  })

  return analysis
}

const exportGraph = async (
  title: string,
  legendLabels: string[],
  yAxisLabel: string,
  nodehun2_xAxisLabel: string,
  nodehun3_xAxisLabel: string,
  nodehun2Times: number[],
  nodehun3Times: number[],
  writeToFile: string
) => {
  return new Promise(resolve => {
    const chartNode = new ChartjsNode(600, 400);

    chartNode.drawChart({
      type: 'bar',
      options: {
        title: {
          display: true,
          text: title
        },
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true
            },
            scaleLabel: {
              display: true,
              labelString: yAxisLabel
            }
          }],
          xAxes: [{
            afterFit: (scale) => {
              scale.height = 40;
            }
          }]
        }
      },
      data: {
        labels: legendLabels,
        datasets: [{
          backgroundColor: '#d9e4aa',
          label: nodehun2_xAxisLabel,
          data: nodehun2Times,
          barThickness: 50,
        }, {
          backgroundColor: '#7ac36a',
          label: nodehun3_xAxisLabel,
          data: nodehun3Times,
          barThickness: 50,
        }]
      }
    })
      .then(() => chartNode.getImageBuffer('image/png'))
      .then(() => chartNode.getImageStream('image/png'))
      .then(() => chartNode.writeImageToFile('image/png', writeToFile))
      .then(resolve)
  })
}

const performanceTest = async () => {
  console.log(`Running performance test`)
  console.log(`...correct word   '${correct}'`)
  console.log(`...incorrect word '${incorrect}'`)
  console.log(`...spelling\n`,
    `......iterations ${iterations_spell}\n`,
    `......samples    ${samples_spell}`)
  console.log(`...suggesting\n`,
    `......iterations ${iterations_suggest}\n`,
    `......samples    ${samples_suggest}`)

  const results_spellSync = []
  const results_spell = []
  const results_suggestSync = []
  const results_suggest = []

  for (let i = 0; i < samples_spell; i++) {
    results_spellSync.push(spellSyncTest(iterations_spell))
    results_spell.push(await spellTest(iterations_spell))
  }

  for (let i = 0; i < samples_suggest; i++) {
    results_suggestSync.push(suggestSyncTest(iterations_suggest))
    results_suggest.push(await suggestTest(iterations_suggest))
  }

  const analysis_spellSync = analyzeResults(results_spellSync)
  const analysis_spell = analyzeResults(results_spell)
  const analysis_suggestSync = analyzeResults(results_suggestSync)
  const analysis_suggest = analyzeResults(results_suggest)

  console.log('Nodehun3#spellSync vs Nodehun2#isCorrectSync')
  console.log(analysis_spellSync)
  console.log('Nodehun3#spell vs Nodehun2#isCorrect')
  console.log(analysis_spell)
  console.log('Nodehun3#suggestSync vs Nodehun2#spellSuggestionsSync')
  console.log(analysis_suggestSync)
  console.log('Nodehun3#suggest vs Nodehun2#spellSuggestions')
  console.log(analysis_suggest)

  await exportGraph(
    `Nodehun3#spell[Sync] vs Nodehun2#isCorrect[Sync] (${samples_spell} samples, ${dictionaryType})`,
    [
      ` correct word\n"${correct}" (sync)`,
      ` correct word\n"${correct}" (async)`,
      ` incorrect word\n"${incorrect}" (sync)`,
      ` incorrect word\n"${incorrect}" (async)`
    ],
    `average milliseconds for ${iterations_spell} operations`,
    'Nodehun2 (w/ promise wrapper for async)',
    'Nodehun3',
    [
      analysis_spellSync.time2_correct.average,
      analysis_spell.time2_correct.average,
      analysis_spellSync.time2_incorrect.average,
      analysis_spell.time2_incorrect.average
    ],
    [
      analysis_spellSync.time3_correct.average,
      analysis_spell.time3_correct.average,
      analysis_spellSync.time3_incorrect.average,
      analysis_spell.time3_incorrect.average
    ],
    './test/performance/spell.png'
  )

  await exportGraph(
    `Nodehun3#suggest[Sync] vs Nodehun2#spellSuggestions[Sync] (${samples_suggest} samples, ${dictionaryType})`,
    [
      ` correct word\n"${correct}" (sync)`,
      ` correct word\n"${correct}" (async)`,
      ` incorrect word\n"${incorrect}" (sync)`,
      ` incorrect word\n"${incorrect}" (async)`
    ],
    `average milliseconds for ${iterations_suggest} operations`,
    'Nodehun2 (w/ promise wrapper for async)',
    'Nodehun3',
    [
      analysis_suggestSync.time2_correct.average,
      analysis_suggest.time2_correct.average,
      analysis_suggestSync.time2_incorrect.average,
      analysis_suggest.time2_incorrect.average
    ],
    [
      analysis_suggestSync.time3_correct.average,
      analysis_suggest.time3_correct.average,
      analysis_suggestSync.time3_incorrect.average,
      analysis_suggest.time3_incorrect.average
    ],
    './test/performance/suggest.png'
  )


}

performanceTest()

