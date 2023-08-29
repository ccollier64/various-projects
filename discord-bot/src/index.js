const { Client, IntentsBitField } = require('discord.js');
const attachment = "../waltuh.jpg";


const client = new Client({
    intents: [
        IntentsBitField.Flags.Guilds,
        IntentsBitField.Flags.GuildMembers,
        IntentsBitField.Flags.GuildMessages,
        IntentsBitField.Flags.MessageContent
    ],

});

client.on('ready', (c) => {
    console.log(`${c.user.tag} is online!`);
});


client.on('messageCreate', (msg) => {

    let pattern1 = /gods/;
    let pattern2 = /a god/;
    let pattern3 = /god's/;
    let pattern4 = /\sgod/;
    let pattern5 = /^god/;

    if(pattern1.test(msg.content)){
        //I can't figure out how to just skip it, so I'm just printing to console so no one sees
        console.log('This is fine');
    }
    else if(pattern2.test(msg.content)){
        console.log('This is fine');
    }
    else if(pattern3.test(msg.content)){
        //If you want to just send a regular message
        msg.reply('Capitalize the G in God!!!!!')

        //If you want to send an attachment of your choice
        //msg.reply({files:[attachment]});
    }
    else if(pattern4.test(msg.content)){
         msg.reply('Capitalize the G in God!!!!!');
         //msg.reply({files:[attachment]});
    }
    else if(pattern5.test(msg.content)){
         msg.reply('Capitalize the G in God!!!!!');
         //msg.reply({files:[attachment]});
    }

});

//Discord bot token goes here
client.login();
