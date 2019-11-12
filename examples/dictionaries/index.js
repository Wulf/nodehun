const fs = require('fs')
const path = require('path')

module.exports = {
    en_US: {
        affix: fs.readFileSync(path.resolve(__dirname, './en_US.aff')),
        dictionary: fs.readFileSync(path.resolve(__dirname, './en_US.dic'))
    },
    en_CA: {
        affix: fs.readFileSync(path.resolve(__dirname, './en_CA.aff')),
        dictionary: fs.readFileSync(path.resolve(__dirname, './en_CA.dic'))
    }
}