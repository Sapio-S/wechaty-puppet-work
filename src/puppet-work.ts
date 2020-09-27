import {
    MessagePayload, 
	MessageType,
    Puppet,
    PuppetOptions,
    ImageType,
    MiniProgramPayload,
    UrlLinkPayload,
    FileBox,
	EventMessagePayload,
	ContactPayload,
	ContactGender,
	ContactType,
	RoomPayload,
} from 'wechaty-puppet'

import ffi = require('ffi')

import fs = require('fs');

var libSDK = ffi.Library('./demo.dll', {
	'getchat': [ 'string', ['int', 'int', "int"]],
	'initial': ['void', ['string', 'string']],
	'destroy': ['void', []],
	'getMedia': ['int', ['int','string', 'string', 'string']],
	'removeFile': ['int', ['string']],
	'getMedia2': ['string', ['int', 'string']],
})

export type PuppetWorkOptions = PuppetOptions

class PuppetWork extends Puppet {
    
	// fake login
	id = "on"
	seq = 79360
	msgNum = 0
	freq = 100 // 100ms 进行一次拉取
	
    constructor(
        public options: PuppetWorkOptions = {},
    ) {
        super(options)
        console.log("puppetWork", "constructor()")
    }

    async start(): Promise<void> {
		console.log('Puppet', 'start()')

		if (this.state.on()) {
			console.log('Puppet', 'start() is called on a ON puppet. await ready(on) and return.')
			await this.state.ready('on')
			return
		}

		this.state.on('pending')
		if (fs.existsSync('parameters.json'))
		{
			let para = JSON.parse(fs.readFileSync("parameters.json",'utf-8'));
			const corpid = para['corpid'];
			const secret = para['secret'];
			libSDK.initial(corpid, secret)
			
			this.state.on(true)
			const eventMessagePayload: EventMessagePayload = {messageId: this.msgNum.toString()} 
			this.emit('message', eventMessagePayload)
		}
		else{
			console.log("can't find parameters.json. Please check your files. ")
		}
    }
	
	private loadmessage(){// ?????
		const eventMessagePayload: EventMessagePayload = {
			messageId: "0",
		}
		this.emit('message', eventMessagePayload)
	}
	
    async stop(): Promise<void> {
        console.log('Puppet', 'stop()')
	
		if (this.state.off()) {
			console.log('Puppet', 'stop() is called on a OFF puppet. await ready(off) and return.')
			await this.state.ready('off')
			return
		}

		this.state.off('pending')
		libSDK.destroy()

		if (this.logonoff()) {
			await this.logout()
		}

		this.state.off(true)
    }
	
	protected async messageRawPayload(messageId: string): Promise<any> {
		return messageId
    }
	
	messageContact(messageId: string): Promise<string> {
        throw new Error("message contact Method not implemented.")
    }
	
	public async clearCache(filename: string): Promise<void>{
		var folderPath = "cache/"
		const fs = require('fs');
		const path = require('path');
		let forlder_exists = fs.existsSync(folderPath);
		
		if (forlder_exists) {
			console.log("clearing caches")
			let fileList = fs.readdirSync(folderPath);
			fileList.forEach(function (fileName) {
				fs.unlinkSync(path.join(folderPath, fileName));
			});
		}
	}
	
    public async messageFile(messageId: string): Promise<FileBox> {
		var seqq = parseInt(messageId)
		let content
		var message = libSDK.getchat(0, 1, seqq)
		while(true){
			try {
				content = JSON.parse(message)
				this.seq = this.seq + 1
				break
			}catch(e) {
				message = libSDK.getchat(1, 0, seqq) // 重新获取缓存的字符串
			}
		}
		let filebox
		let result
		switch (content.msgtype){
			case "image":
				result = libSDK.getMedia(content.image.filesize, content.image.sdkfileid, 
				"cache/"+content.msgid+".jpg", content.image.md5sum);
				filebox = FileBox.fromFile("cache/"+content.msgid+".jpg")
				filebox.path = "."
				return filebox
			case "voice":
				result = libSDK.getMedia(content.voice.voice_size,content.voice.sdkfileid, 
				"cache/"+content.msgid+".amr", content.voice.md5sum);
				filebox = FileBox.fromFile("cache/"+content.msgid+".amr")
				filebox.path = "."
				return filebox
			case "video":
				result = libSDK.getMedia(content.video.filesize,content.video.sdkfileid, 
				"cache/"+content.msgid+".mp4", content.video.md5sum);
				filebox = FileBox.fromFile("cache/"+content.msgid+".mp4")
				filebox.path = "."
				return filebox
			case "emotion":
				if(content.type == 1){
					result = libSDK.getMedia(content.emotion.imagesize,content.emotion.sdkfileid, 
					"cache/"+content.msgid+".gif", content.emotion.md5sum);
					filebox = FileBox.fromFile("cache/"+content.msgid+".gif")
					filebox.path = "."
					return filebox
				}
				else{
					result = libSDK.getMedia(content.emotion.imagesize,content.emotion.sdkfileid, 
					"cache/"+content.msgid+".png", content.emotion.md5sum);
					filebox = FileBox.fromFile("cache/"+content.msgid+".png")
					filebox.path = "."
					return filebox
				}
			case "file":
				var filename = content.file.filename+"."+content.file.fileext
				result = libSDK.getMedia(content.file.filesize,content.file.sdkfileid, 
				"cache/"+filename, content.file.md5sum);
				filebox = FileBox.fromFile("cache/"+filename)
				filebox.path = "."
				return filebox
			default:
				const base64 ='Tm90IFN1cHBvcnRlZCBBdHRhY2htZW50IEZpbGUgVHlwZSBpbiBNZXNzYWdlLgpTZWU6IGh0dHBzOi8vZ2l0aHViLmNvbS9DaGF0aWUvd2VjaGF0eS9pc3N1ZXMvMTI0OQo='
				filename = 'wechaty-puppet-padplus-message-attachment-' + seqq.toString() + '.txt'
				return FileBox.fromBase64(base64,filename)
		}
    }
	
    messageImage(messageId: string, imageType: ImageType): Promise<FileBox> {
        throw new Error("Method not implemented.")
    }
    messageMiniProgram(messageId: string): Promise<MiniProgramPayload> {
        throw new Error("Method not implemented.")
    }
    messageUrl(messageId: string): Promise<UrlLinkPayload> {
        throw new Error("Method not implemented.")
    }
	
    public async messageRawPayloadParser(rawPayload: MessagePayload): Promise<MessagePayload> {
		var message = libSDK.getchat(0, 1, this.seq)
		let content
		
		while(true){
			try {
				content = JSON.parse(message)
				this.seq = this.seq + 1
				break
			}catch(e) {
				message = libSDK.getchat(1, 0, this.seq) // 重新获取缓存的字符串
			}
		}

		if(content.action == "switch"){
			var payload0 : MessagePayload = {
				id: content.msgid, 
				timestamp: content.time, 
				type: MessageType.Text, 
				fromId: content.user, 
				toId: "None", 
				text: "Switch"} 
			setTimeout(() => this.emit('message', {messageId: (this.seq).toString()}), this.freq)
			return payload0
		}

		let fromId0: undefined | string
		let roomId0: undefined | string
		let toId0:   undefined | string
		let payload: MessagePayload
		
		
		if(content.tolist.length > 1){
			var list2 = ""
			content.tolist.forEach(function(element) {
				list2 = list2 + element+" "
			});
			roomId0 = content.roomid+" "+list2
			//toId0 = content.tolist[0]
		}
		else{
			toId0 = content.tolist[0]
		}

		fromId0 = content.from
		payload = {
			id: content.msgid, 
			timestamp: content.msgtime, 
			fromId: fromId0,
			toId: toId0,
			roomId: roomId0,
			type: MessageType.Text}

		
		switch (content.msgtype){
			case "markdown":
				payload.type = MessageType.Text
				payload.text = content.info.content
				break
			case "text":
				payload.type = MessageType.Text
				payload.text = content.text.content
				break
			case "card": // ??
				payload.type = MessageType.Text
				payload.text = "card. corpname: "+content.card.corpname+" ; userid: "+content.card.userid
				break
			case "revoke":
				payload.type = MessageType.Text
				payload.text = "revoke a message. The origianl message id is"+content.pre_msgid
				break
			case "location":
				payload.type = MessageType.Location
				payload.text = "location: "+content.location.address
				break
			case "link":
				payload.type = MessageType.Url
				payload.text = "link. title: "+content.link.title+" description: "+content.link.description
				break
			case "weapp":
				payload.type = MessageType.MiniProgram
				payload.text = "小程序。 title: "+content.weapp.title+" description: "+content.weapp.description
				break
			case "chatrecord":
				payload.type = MessageType.Text
				payload.text = "聊天记录。 title: "+content.chatrecord.title+" items: "+content.chatrecord.descriptiont
				break
			case "todo":
				payload.type = MessageType.Text
				payload.text = "待办. title: "+content.todo.title+" content: "+content.todo.content
				break
			case "image":
				payload.type = MessageType.Image
				break
			case "voice":
				payload.type = MessageType.Audio
				break
			case "video":
				payload.type = MessageType.Video
				break
			case "emotion":
				payload.type = MessageType.Emoticon
				break
			case "file":
				payload.type = MessageType.Attachment
				break
			default:
				payload.type = MessageType.Unknown
				payload.text = "Sorry, we don't support this kind of message right now. The message is "+content.msgtype
		}

		setTimeout(() => this.emit('message', {messageId: (this.seq).toString()}), this.freq)
		return payload
    }
	
	protected async contactRawPayload(contactId: string): Promise<any> {
		//console.log("puppetWork", "contactRawPayload()")
		return contactId
    }
	
    protected async contactRawPayloadParser(rawPayload: any): Promise<ContactPayload> {
		/* console.log("puppetWork", "contactRawPayloadParser()")
		console.log(rawPayload) */
		let contactType = ContactType.Unknown
 		const payload: ContactPayload = {
			alias     : "",
			avatar    : "",
			city      : "",
			gender    : ContactGender.Unknown,
			id        : "",
			name      : rawPayload,
			province  : "",
			signature : "",
			type      : contactType,
			weixin    : "",
		} 
		return payload
    }
	
	protected async roomRawPayload(roomId: string): Promise<any> {
		//console.log("puppetWork", "roomRawPayload()")
        return roomId
    }
	
    protected async roomRawPayloadParser(rawPayload: any): Promise<RoomPayload> {
		//console.log("puppetWork", "roomRawPayloadParser()")
		//console.log(rawPayload)
		let tmp
		tmp = rawPayload.split(" ")
		const payload: RoomPayload = {
			id:  tmp[0],
			avatar:"",
			topic:"",
			memberIdList:tmp.slice(1,),
			adminIdList:[],
		}
		return payload
    }
	
	public async roomMemberList(roomId: string): Promise<string[]> {
		//console.log("roomMemberList")
		//console.log(roomId)
		let tmp
		tmp = roomId.split(" ")
		return tmp.slice(1,)
    }
	
    protected roomMemberRawPayload(roomId: string, contactId: string): Promise<any> {
		throw new Error("roomMemberRawPayload Method not implemented.")
       /*  console.log("roomMemberRawPayload")
		console.log(roomId, contactId)
		return '' */
		//return roomId
    }
    protected roomMemberRawPayloadParser(rawPayload: any): Promise<import("wechaty-puppet").RoomMemberPayload> {
		throw new Error("roomMemberRawPayloadParser Method not implemented.")
        /* console.log("roomMemberRawPayloadParser")
		console.log(rawPayload)
		return rawPayload */
    }
	
	public async login(): Promise<void> {
		console.log("puppetWork", "login()")
		this.id = "on"
    }
	
    public async logout(): Promise<void> {
		console.log("puppetWork", "logout()")
		this.id = "off"
    }
	
    ding(data?: string): void {
        throw new Error(" ding Method not implemented.")
    }
	
    contactSelfName(name: string): Promise<void> {
        throw new Error("contactSelfName Method not implemented.")
    }
    contactSelfQRCode(): Promise<string> {
        throw new Error("Method not implemented.")
    }
    contactSelfSignature(signature: string): Promise<void> {
        throw new Error("contactSelfSignature Method not implemented.")
    }
    tagContactAdd(tagId: string, contactId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    tagContactDelete(tagId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    tagContactList(tagId: string, contactId: string): Promise<string[]>;
    tagContactList(): Promise<string[]>
    tagContactList(contactId?: any): Promise<string[]> | null {
        throw new Error("tagContactList Method not implemented.")
    }
    tagContactRemove(tagId: string, contactId: string): Promise<void> {
        throw new Error("tagContactRemove Method not implemented.")
    }
    contactAlias(contactId: string): Promise<string>
    contactAlias(contactId: string, alias: string): Promise<void>
    contactAlias(contactId: any, alias?: any): Promise<void> | Promise<string> {
        throw new Error("contactAlias Method not implemented.")
    }
    contactAvatar(contactId: string): Promise<FileBox>
    contactAvatar(contactId: string, file: FileBox): Promise<void>
    contactAvatar(contactId: any, file?: any): Promise<FileBox> | Promise<void> {
        throw new Error("contactAvatar Method not implemented.")
    }
    contactList(): Promise<string[]> {
        throw new Error("contactList Method not implemented.")
    }

    friendshipAccept(friendshipId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    friendshipAdd(contactId: string, hello?: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    friendshipSearchPhone(phone: string): Promise<string> {
        throw new Error("Method not implemented.")
    }
    friendshipSearchWeixin(weixin: string): Promise<string> {
        throw new Error("Method not implemented.")
    }
    protected friendshipRawPayload(friendshipId: string): Promise<any> {
        throw new Error("friendshipRawPayload Method not implemented.")
    }
    protected friendshipRawPayloadParser(rawPayload: any): Promise<import("wechaty-puppet").FriendshipPayload> {
        throw new Error("friendshipRawPayloadParser Method not implemented.")
    }
	
    messageSendContact(conversationId: string, contactId: string): Promise<string | void> {
        throw new Error("messageSendContact Method not implemented.")
    }
    messageSendFile(conversationId: string, file: FileBox): Promise<string | void> {
        throw new Error("Method not implemented.")
    }
    messageSendMiniProgram(conversationId: string, miniProgramPayload: MiniProgramPayload): Promise<string | void> {
        throw new Error("Method not implemented.")
    }
    messageSendText(conversationId: string, text: string, mentionIdList?: string[]): Promise<string | void> {
        throw new Error("messageSendText Method not implemented.")
    }
    messageSendUrl(conversationId: string, urlLinkPayload: UrlLinkPayload): Promise<string | void> {
        throw new Error("Method not implemented.")
    }
    messageRecall(messageId: string): Promise<boolean> {
        throw new Error(" messageRecall messageRecall Method not implemented.")
    }

    roomInvitationAccept(roomInvitationId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    protected roomInvitationRawPayload(roomInvitationId: string): Promise<any> {
        throw new Error("Method not implemented.")
    }
    protected roomInvitationRawPayloadParser(rawPayload: any): Promise<import("wechaty-puppet").RoomInvitationPayload> {
        throw new Error("Method not implemented.")
    }
    roomAdd(roomId: string, contactId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    roomAvatar(roomId: string): Promise<FileBox> {
        throw new Error("room avatar Method not implemented.")
    }
    roomCreate(contactIdList: string[], topic?: string): Promise<string> {
        throw new Error("room create Method not implemented.")
    }
    roomDel(roomId: string, contactId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    roomList(): Promise<string[]> {
        throw new Error("room list Method not implemented.")
    }
    roomQRCode(roomId: string): Promise<string> {
        throw new Error("Method not implemented.")
    }
    roomQuit(roomId: string): Promise<void> {
        throw new Error("Method not implemented.")
    }
    roomTopic(roomId: string): Promise<string>
    roomTopic(roomId: string, topic: string): Promise<void>
    roomTopic(roomId: any, topic?: any): Promise<string> | Promise<void> {
        throw new Error("roomTopic Method not implemented.")
    }
    
    roomAnnounce(roomId: string): Promise<string>
    roomAnnounce(roomId: string, text: string): Promise<void>
    roomAnnounce(roomId: any, text?: any): Promise<string> | Promise<void> {
        throw new Error("Method not implemented.")
    }


}

export { PuppetWork }
export default PuppetWork
