/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	au_language.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-08-30
  Description:  
				GUI语言处理程序头文件
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __AU_LANGUAGE_H__
#define __AU_LANGUAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

//将自己用到的字符串移到所属分类
//如果在这里面没有相应的字符串，请自行在所属分类末尾按格式添加!

// 主界面文字
#define	ESID_NULL						 (g_China ? (g_Big5 ? "才" : "空字符 ") : "NULL")
#define	ESID_MainAlarm					 (g_China ? (g_Big5 ? "ň" : "安防") : "Security")
#define	ESID_MainCall					 (g_China ? (g_Big5 ? "癸量" : "对讲") : "Intercom")
#define ESID_MainInfo				     (g_China ? (g_Big5 ? "癟" : "信息") : "Information")
#define ESID_MainCenter                  (g_China ? (g_Big5 ? "㊣いみ" : "呼叫中心") : "CALL ADMIN")
#define ESID_MainMonitor			     (g_China ? (g_Big5 ? "菏跌" : "监视") : "Monitor")
#define ESID_MainRecord			         (g_China ? (g_Big5 ? "硄杠癘魁" : "通话记录") : "Calls Register")
#define ESID_MainLyLy		             (g_China ? (g_Big5 ? "痙紇痙ē" : "留影留言") : "MSG")
#define ESID_MainBianli					 (g_China ? (g_Big5 ? "獽\" : "便利功能") : "Facility")
#define ESID_MainBianLi_Dianti           (g_China ? (g_Big5 ? "筿辫酬" : "电梯召唤") : "Elevator")
#define ESID_MainSet					 (g_China ? (g_Big5 ? "砞竚" : "设置") : "Settings")
#define ESID_MainEcontrol				 (g_China ? (g_Big5 ? "產筿北" : "家电控制") : "Automation")
#define ESID_MainMedia				     (g_China ? (g_Big5 ? "碈砰" : "多媒体") : "Media")

// 公共文字
#define ESID_Unregistered           	 (g_China ? (g_Big5 ? "ゼ爹" : "未注册") : "Unregistered")
#define ESID_Registered           		 (g_China ? (g_Big5 ? "爹" : "已注册") : "Registered")
#define ESID_Touch_Calibration           (g_China ? (g_Big5 ? "牟北棵!" : "触摸屏校准!") : "Screen Calibration")
#define ESID_Touch_Disp_Chk_Hit          (g_China ? (g_Big5 ? "叫'+'いみ'+'铬竚" : "请按住'+'中心直到'+'跳到下一个位置") : "Press and hold '+' until the '+' skip to the next")
#define ESID_clearscreenhit              (g_China ? (g_Big5 ? "叫ノ琗硁睲间珇揽棵辊,逞緇丁:" : "请用柔软清洁的物品擦拭屏幕,剩余时间:") : "Pls clean the screen. remaining time:")
#define ESID_Right_Add                   (g_China ? (g_Big5 ? "睰" : "添加") : "Add")
#define ESID_Right_Edit                  (g_China ? (g_Big5 ? "絪胯" : "编辑") : "Edit")
#define ESID_Right_Del                   (g_China ? (g_Big5 ? "埃" : "删除") : "Delete")
#define ESID_Right_Clear                 (g_China ? (g_Big5 ? "睲" : "清空") : "Empty")
#define ESID_Right_Save                  (g_China ? (g_Big5 ? "玂" : "保存") : "Save")
#define ESID_Right_View                  (g_China ? (g_Big5 ? "琩" : "查看") : "View")
#define ESID_Right_Sure                  (g_China ? (g_Big5 ? "絋粄" : "确认") : "OK")
#define ESID_Right_desk                  (g_China ? (g_Big5 ? "砞竚" : "设置桌面") : "Set")
#define ESID_Right_Rec                   (g_China ? (g_Big5 ? "魁籹" : "录制") : "Record")
#define ESID_Right_Listen                (g_China ? (g_Big5 ? "刚钮" : "试听") : "Audition")
#define ESID_Right_Play                  (g_China ? (g_Big5 ? "冀" : "播放") : "Play")
#define ESID_Right_Stop                  (g_China ? (g_Big5 ? "氨ゎ" : "停止") : "Stop")
#define ESID_Right_Pause                 (g_China ? (g_Big5 ? "既氨" : "暂停") : "Pause")
#define ESID_Right_Updata                (g_China ? (g_Big5 ? "˙" : "同步") : "Synchronize")
#define ESID_Right_Select                (g_China ? (g_Big5 ? "匡拒" : "选择") : "Select")
#define ESID_Right_Input                 (g_China ? (g_Big5 ? "块" : "输入") : "Input")
#define ESID_Week_Sun                    (g_China ? (g_Big5 ? "琍戳ら" : "星期日") : "Sun.")
#define ESID_Week_Mon                    (g_China ? (g_Big5 ? "琍戳" : "星期一") : "Mon.")
#define ESID_Week_Tue                    (g_China ? (g_Big5 ? "琍戳" : "星期二") : "Tues.")
#define ESID_Week_Wed                    (g_China ? (g_Big5 ? "琍戳" : "星期三") : "Wed.")
#define ESID_Week_Thu                    (g_China ? (g_Big5 ? "琍戳" : "星期四") : "Thur.")
#define ESID_Week_Fri                    (g_China ? (g_Big5 ? "琍戳き" : "星期五") : "Fri.")
#define ESID_Week_Sat                    (g_China ? (g_Big5 ? "琍戳せ" : "星期六") : "Sat.")
#define ESID_Time_Sec                    (g_China ? (g_Big5 ? "" : "秒") : "Sec")
#define ESID_Time_Hour                   (g_China ? (g_Big5 ? "" : "小时") : "Hour")
#define ESID_Time_Min                    (g_China ? (g_Big5 ? "だ" : "分") : "Min")
#define ESID_Time_Never					 (g_China ? (g_Big5 ? "眖ぃ" : "从不") : "Never")
#define ESID_Set_Time                    (g_China ? (g_Big5 ? ":" : "时长:") : "Duration")
#define ESID_Set_Mode                    (g_China ? (g_Big5 ? "家Α:" : "模式:") : "Mode")
#define ESID_Set_Used                    (g_China ? (g_Big5 ? "币ノ" : "启用") : "Activate")
#define ESID_Save_OK                     (g_China ? (g_Big5 ? "玂  Θ \!" : "保 存 成 功!") : "OK!")
#define ESID_Save_ERR                    (g_China ? (g_Big5 ? "玂  ア 毖!" : "保 存 失 败!") : "Failed!")
#define ESID_Msg_Yes                     (g_China ? (g_Big5 ? "琌" : "是") : "Yes")
#define ESID_Msg_No                      (g_China ? (g_Big5 ? "" : "否") : "No")
#define ESID_Msg_Format                  (g_China ? (g_Big5 ? "叫絋粄琌Αて?" : "请确认是否格式化?") : "Sure to format disk?")
#define ESID_Oper_OK                     (g_China ? (g_Big5 ? "巨  Θ \!" : "操 作 成 功!") : "OK!")
#define ESID_Oper_ERR                    (g_China ? (g_Big5 ? "巨  ア 毖!" : "操 作 失 败!") : "Failed!")
#define ESID_Msg_Calibration             (g_China ? (g_Big5 ? "琌秈︽牟篘非?" : "是否进行触摸屏校准?") : "Are you sure to calibrate the screen?")
#define ESID_Msg_OldPass_Err             (g_China ? (g_Big5 ? "侣盞絏岿粇!" : "旧密码错误!") : "Wrong old PIN!")
#define ESID_Msg_SamePass_Err            (g_China ? (g_Big5 ? "ㄢΩ块盞絏ぃ!" : "两次输入的密码不相同!") : "Pls re-enter the same new PIN!")
#define ESID_Msg_InputBox_Psw_Err        (g_China ? (g_Big5 ? "盞絏岿粇,叫穝块!" : "密码错误,请重新输入!") : "Wrong Password!")
#define ESID_Msg_Jrly_rec_tmp            (g_China ? (g_Big5 ? "琌埃魁籹ゅン?" : "是否删除已录制的文件?") : "Sure to delete it?")
#define ESID_Msg_Jrly_rec_full           (g_China ? (g_Big5 ? "產痙ē丁骸!" : "家人留言空间已满!") : "Memory Full!")
#define ESID_Msg_Connect_Outtime         (g_China ? (g_Big5 ? "硈钡禬" : "连接超时") : "Connection timeout!")
#define ESID_Msg_Alarm_Hit               (g_China ? (g_Big5 ? "叫猔種,┑ň跋牟祇!" : "请注意,延时防区触发!") : "Warning! alarm triggered!")
#define ESID_Msg_Info_Hit                (g_China ? (g_Big5 ? "眤,Τ穝癟!" : "您好,有新的信息!") : "Hi,you have new message!")
#define ESID_Msg_CallIn_Hit              (g_China ? (g_Big5 ? "眤,Τ穝ㄓ筿!" : "您好,有新的来电!") : "Hi,you have new call now!")
#define ESID_Msg_FactoryMode             (g_China ? (g_Big5 ? "琌璶確紅砞竚?" : "是否要恢复出厂设置?") : "Sure to restore factory settings?")
#define ESID_Msg_Cell_Err                (g_China ? (g_Big5 ? "虫じ獶猭" : "单元长度非法") : "Illegal Riser No.!")
#define ESID_Msg_Stair_Err               (g_China ? (g_Big5 ? "辫獶猭" : "梯号长度非法") : "Illegal Stairway No.!")
#define ESID_Msg_Room_Err                (g_China ? (g_Big5 ? "┬腹獶猭" : "房号长度非法") : "Illegal Apartment No.!")
#define ESID_Msg_RuleAll_Err             (g_China ? (g_Big5 ? "羆ぃ18" : "总长度不能大于18") : "Length should be less than 18")
#define ESID_Msg_Len_Err                 (g_China ? (g_Big5 ? "砞称絪腹獶猭" : "设备编号长度非法") : "Illegal Length!")
#define ESID_Msg_RoomNo_Err              (g_China ? (g_Big5 ? "┬腹琿0" : "房号段全为0") : "Room No. can not be all zero!")
#define ESID_Msg_Recover_OK              (g_China ? (g_Big5 ? "巨Θ\,╰参盢币!" : "操作成功,系统将重启!") : "OK! The system will restart!")
#define ESID_Msg_Exit                    (g_China ? (g_Big5 ? "癶" : "退出 ") : "Exit")
#define ESID_Msg_Dianti_Hit1             (g_China ? (g_Big5 ? "筿辫琌そ砞琁,產ㄏノウ," : "电梯是公用设施,为了大家能更好的使用它,") : "In order to make better use of elevator")
#define ESID_Msg_Dianti_Hit2             (g_China ? (g_Big5 ? "叫眤ぃ璶繦種酬!" : "请您不要随意召唤!") : "Please do not arbitrarily call it!")
#define ESID_Msg_Reset                   (g_China ? (g_Big5 ? "╰参币い..." : "系统重启中...") : "System Reset...")
#define ESID_Msg_JD_Is_Updata            (g_China ? (g_Big5 ? "˙盢穝癟,琌˙?" : "同步后将更新原信息,是否同步?") : "All data will be renewed.")
#define ESID_Msg_JD_Is_Updata1           (g_China ? (g_Big5 ? "˙盢穝癟,琌˙?" : "同步后将更新原信息,是否同步?") : " Sure to synchronize? ")
#define ESID_Msg_JD_Updataing            (g_China ? (g_Big5 ? "˙產筿癟い,叫祔..." : "同步家电信息中,请稍后...") : "Settings data synchronizing...")
#define ESID_Msg_JD_Unbind               (g_China ? (g_Big5 ? "IP﹚锣传竟ゼ竕﹚,ぃ˙!" : "IP协议转换器未绑定,不能同步!") : "Cannot synchronize!")
#define ESID_Msg_Create_BindCode         (g_China ? (g_Big5 ? "眤絋﹚璶ネΘ穝竕﹚盾?" : "您确定要生成新的绑定码吗?") : "Sure to generate new binding code")
#define ESID_IP_Unbind                   (g_China ? (g_Big5 ? "IP﹚锣传竟ゼ竕﹚" : "IP协议转换器未绑定") : "Not bound to IP converter yet.")
#define ESID_IP_Bind                     (g_China ? (g_Big5 ? "IP﹚锣传竟竕﹚Θ\!" : "IP协议转换器绑定成功!") : "Bound to IP converter.")
#define ESID_Msg_Jd_State_Updata         (g_China ? (g_Big5 ? "產筿篈˙い,叫祔..." : "家电设备状态同步中,请稍后...") : "Device status synchronizing...")
#define ESID_IP_Online                   (g_China ? (g_Big5 ? "IP﹚锣传竟絬" : "IP协议转换器在线") : "IP Converter Online.")
#define ESID_IP_Unline                   (g_China ? (g_Big5 ? "IP﹚锣传竟ゼ絬" : "IP协议转换器未在线") : "IP Converter Offline.")
#define ESID_IP_Con_Addr                 (g_China ? (g_Big5 ? "IP﹚锣传竟" : "IP协议转换器地址") : "IP Converter Address")
#define ESID_Jd_Deng1                    (g_China ? (g_Big5 ? "縊1" : "灯1") : "Light1")
#define ESID_Jd_Deng2                    (g_China ? (g_Big5 ? "縊2" : "灯2") : "Light2")
#define ESID_Jd_Deng3                    (g_China ? (g_Big5 ? "縊3" : "灯3") : "Light3")
#define ESID_Jd_Deng4                    (g_China ? (g_Big5 ? "縊4" : "灯4") : "Light4")
#define ESID_Jd_Deng5                    (g_China ? (g_Big5 ? "縊5" : "灯5") : "Light5")
#define ESID_Jd_Deng6                    (g_China ? (g_Big5 ? "縊6" : "灯6") : "Light6")
#define ESID_Jd_Deng7                    (g_China ? (g_Big5 ? "縊7" : "灯7") : "Light7")
#define ESID_Jd_Deng8                    (g_China ? (g_Big5 ? "縊8" : "灯8") : "Light8")
#define ESID_Jd_Deng9                    (g_China ? (g_Big5 ? "縊9" : "灯9") : "Light9")
#define ESID_Jd_Deng10                   (g_China ? (g_Big5 ? "縊10" : "灯10") : "Light10")
#define ESID_Jd_TiaoGuangDeng1           (g_China ? (g_Big5 ? "秸縊1" : "调光灯1") : "Dimming Lights1")
#define ESID_Jd_TiaoGuangDeng2           (g_China ? (g_Big5 ? "秸縊2" : "调光灯2") : "Dimming Lights2")
#define ESID_Jd_TiaoGuangDeng3           (g_China ? (g_Big5 ? "秸縊3" : "调光灯3") : "Dimming Lights3")
#define ESID_Jd_TiaoGuangDeng4           (g_China ? (g_Big5 ? "秸縊4" : "调光灯4") : "Dimming Lights4")
#define ESID_Jd_TiaoGuangDeng5           (g_China ? (g_Big5 ? "秸縊5" : "调光灯5") : "Dimming Lights5")
#define ESID_Jd_TiaoGuangDeng6           (g_China ? (g_Big5 ? "秸縊6" : "调光灯6") : "Dimming Lights6")
#define ESID_Jd_TiaoGuangDeng7           (g_China ? (g_Big5 ? "秸縊7" : "调光灯7") : "Dimming Lights7")
#define ESID_Jd_TiaoGuangDeng8           (g_China ? (g_Big5 ? "秸縊8" : "调光灯8") : "Dimming Lights8")
#define ESID_Jd_TiaoGuangDeng9           (g_China ? (g_Big5 ? "秸縊9" : "调光灯9") : "Dimming Lights9")
#define ESID_Jd_TiaoGuangDeng10          (g_China ? (g_Big5 ? "秸縊10" : "调光灯10") : "Dimming Lights10")
#define ESID_Jd_BaiChiDeng1              (g_China ? (g_Big5 ? "フ縆縊1" : "白炽灯1") : "Incandescent Lamp1")
#define ESID_Jd_BaiChiDeng2              (g_China ? (g_Big5 ? "フ縆縊2" : "白炽灯2") : "Incandescent Lamp2")
#define ESID_Jd_BaiChiDeng3              (g_China ? (g_Big5 ? "フ縆縊3" : "白炽灯3") : "Incandescent Lamp3")
#define ESID_Jd_BaiChiDeng4              (g_China ? (g_Big5 ? "フ縆縊4" : "白炽灯4") : "Incandescent Lamp4")
#define ESID_Jd_BaiChiDeng5              (g_China ? (g_Big5 ? "フ縆縊5" : "白炽灯5") : "Incandescent Lamp5")
#define ESID_Jd_BaiChiDeng6              (g_China ? (g_Big5 ? "フ縆縊6" : "白炽灯6") : "Incandescent Lamp6")
#define ESID_Jd_BaiChiDeng7              (g_China ? (g_Big5 ? "フ縆縊7" : "白炽灯7") : "Incandescent Lamp7")
#define ESID_Jd_BaiChiDeng8              (g_China ? (g_Big5 ? "フ縆縊8" : "白炽灯8") : "Incandescent Lamp8")
#define ESID_Jd_BaiChiDeng9              (g_China ? (g_Big5 ? "フ縆縊9" : "白炽灯9") : "Incandescent Lamp9")
#define ESID_Jd_BaiChiDeng10             (g_China ? (g_Big5 ? "フ縆縊10" : "白炽灯10") : "Incandescent Lamp10")
#define ESID_Jd_GuangGuan1               (g_China ? (g_Big5 ? "恨1" : "光管1") : "Lightpipe1")
#define ESID_Jd_GuangGuan2               (g_China ? (g_Big5 ? "恨2" : "光管2") : "Lightpipe2")
#define ESID_Jd_GuangGuan3               (g_China ? (g_Big5 ? "恨3" : "光管3") : "Lightpipe3")
#define ESID_Jd_GuangGuan4               (g_China ? (g_Big5 ? "恨4" : "光管4") : "Lightpipe4")
#define ESID_Jd_GuangGuan5               (g_China ? (g_Big5 ? "恨5" : "光管5") : "Lightpipe5")
#define ESID_Jd_GuangGuan6               (g_China ? (g_Big5 ? "恨6" : "光管6") : "Lightpipe6")
#define ESID_Jd_GuangGuan7               (g_China ? (g_Big5 ? "恨7" : "光管7") : "Lightpipe7")
#define ESID_Jd_GuangGuan8               (g_China ? (g_Big5 ? "恨8" : "光管8") : "Lightpipe8")
#define ESID_Jd_GuangGuan9               (g_China ? (g_Big5 ? "恨9" : "光管9") : "Lightpipe9")
#define ESID_Jd_GuangGuan10              (g_China ? (g_Big5 ? "恨10" : "光管10") : "Lightpipe10")
#define ESID_Jd_DiaoDeng1                (g_China ? (g_Big5 ? "縊1" : "吊灯1") : "Chandelier1")
#define ESID_Jd_DiaoDeng2                (g_China ? (g_Big5 ? "縊2" : "吊灯2") : "Chandelier2")
#define ESID_Jd_DiaoDeng3                (g_China ? (g_Big5 ? "縊3" : "吊灯3") : "Chandelier3")
#define ESID_Jd_DiaoDeng4                (g_China ? (g_Big5 ? "縊4" : "吊灯4") : "Chandelier4")
#define ESID_Jd_DiaoDeng5                (g_China ? (g_Big5 ? "縊5" : "吊灯5") : "Chandelier5")
#define ESID_Jd_DiaoDeng6                (g_China ? (g_Big5 ? "縊6" : "吊灯6") : "Chandelier6")
#define ESID_Jd_DiaoDeng7                (g_China ? (g_Big5 ? "縊7" : "吊灯7") : "Chandelier7")
#define ESID_Jd_DiaoDeng8                (g_China ? (g_Big5 ? "縊8" : "吊灯8") : "Chandelier8")
#define ESID_Jd_DiaoDeng9                (g_China ? (g_Big5 ? "縊9" : "吊灯9") : "Chandelier9")
#define ESID_Jd_DiaoDeng10               (g_China ? (g_Big5 ? "縊10" : "吊灯10") : "Chandelier10")
#define ESID_Jd_HuaDeng1                 (g_China ? (g_Big5 ? "縊1" : "花灯1") : "Lantern1")
#define ESID_Jd_HuaDeng2                 (g_China ? (g_Big5 ? "縊2" : "花灯2") : "Lantern2")
#define ESID_Jd_HuaDeng3                 (g_China ? (g_Big5 ? "縊3" : "花灯3") : "Lantern3")
#define	ESID_Jd_HuaDeng4                 (g_China ? (g_Big5 ? "縊4" : "花灯4") : "Lantern4")
#define ESID_Jd_HuaDeng5                 (g_China ? (g_Big5 ? "縊5" : "花灯5") : "Lantern5")
#define ESID_Jd_HuaDeng6                 (g_China ? (g_Big5 ? "縊6" : "花灯6") : "Lantern6")
#define ESID_Jd_HuaDeng7                 (g_China ? (g_Big5 ? "縊7" : "花灯7") : "Lantern7")
#define ESID_Jd_HuaDeng8                 (g_China ? (g_Big5 ? "縊8" : "花灯8") : "Lantern8")
#define ESID_Jd_HuaDeng9                 (g_China ? (g_Big5 ? "縊9" : "花灯9") : "Lantern9")
#define ESID_Jd_HuaDeng10                (g_China ? (g_Big5 ? "縊10" : "花灯10") : "Lantern10")
#define ESID_Jd_BiDeng1                  (g_China ? (g_Big5 ? "纠縊1" : "壁灯1") : "Wall Lamp1")
#define ESID_Jd_BiDeng2                  (g_China ? (g_Big5 ? "纠縊2" : "壁灯2") : "Wall Lamp2")
#define ESID_Jd_BiDeng3                  (g_China ? (g_Big5 ? "纠縊3" : "壁灯3") : "Wall Lamp3")
#define ESID_Jd_BiDeng4                  (g_China ? (g_Big5 ? "纠縊4" : "壁灯4") : "Wall Lamp4")
#define ESID_Jd_BiDeng5                  (g_China ? (g_Big5 ? "纠縊5" : "壁灯5") : "Wall Lamp5")
#define ESID_Jd_BiDeng6                  (g_China ? (g_Big5 ? "纠縊6" : "壁灯6") : "Wall Lamp6")
#define ESID_Jd_BiDeng7                  (g_China ? (g_Big5 ? "纠縊7" : "壁灯7") : "Wall Lamp7")
#define ESID_Jd_BiDeng8                  (g_China ? (g_Big5 ? "纠縊8" : "壁灯8") : "Wall Lamp8")
#define ESID_Jd_BiDeng9                  (g_China ? (g_Big5 ? "纠縊9" : "壁灯9") : "Wall Lamp9")
#define ESID_Jd_BiDeng10                 (g_China ? (g_Big5 ? "纠縊10" : "壁灯10") : "Wall Lamp10")
#define ESID_Jd_SheDeng1                 (g_China ? (g_Big5 ? "甮縊1" : "射灯1") : "Spotlight1")
#define ESID_Jd_SheDeng2                 (g_China ? (g_Big5 ? "甮縊2" : "射灯2") : "Spotlight2")
#define ESID_Jd_SheDeng3                 (g_China ? (g_Big5 ? "甮縊3" : "射灯3") : "Spotlight3")
#define ESID_Jd_SheDeng4                 (g_China ? (g_Big5 ? "甮縊4" : "射灯4") : "Spotlight4")
#define ESID_Jd_SheDeng5                 (g_China ? (g_Big5 ? "甮縊5" : "射灯5") : "Spotlight5")
#define ESID_Jd_SheDeng6                 (g_China ? (g_Big5 ? "甮縊6" : "射灯6") : "Spotlight6")
#define ESID_Jd_SheDeng7                 (g_China ? (g_Big5 ? "甮縊7" : "射灯7") : "Spotlight7")
#define ESID_Jd_SheDeng8                 (g_China ? (g_Big5 ? "甮縊8" : "射灯8") : "Spotlight8")
#define ESID_Jd_SheDeng9                 (g_China ? (g_Big5 ? "甮縊9" : "射灯9") : "Spotlight9")
#define ESID_Jd_SheDeng10                (g_China ? (g_Big5 ? "甮縊10" : "射灯10") : "Spotlight10")
#define ESID_Jd_TaiDeng1                 (g_China ? (g_Big5 ? "籓縊1" : "台灯1") : "Table Lamp1")
#define ESID_Jd_TaiDeng2                 (g_China ? (g_Big5 ? "籓縊2" : "台灯2") : "Table Lamp2")
#define ESID_Jd_TaiDeng3                 (g_China ? (g_Big5 ? "籓縊3" : "台灯3") : "Table Lamp3")
#define ESID_Jd_TaiDeng4                 (g_China ? (g_Big5 ? "籓縊4" : "台灯4") : "Table Lamp4")
#define ESID_Jd_TaiDeng5                 (g_China ? (g_Big5 ? "籓縊5" : "台灯5") : "Table Lamp5")
#define ESID_Jd_TaiDeng6                 (g_China ? (g_Big5 ? "籓縊6" : "台灯6") : "Table Lamp6")
#define ESID_Jd_TaiDeng7                 (g_China ? (g_Big5 ? "籓縊7" : "台灯7") : "Table Lamp7")
#define ESID_Jd_TaiDeng8                 (g_China ? (g_Big5 ? "籓縊8" : "台灯8") : "Table Lamp8")
#define ESID_Jd_TaiDeng9                 (g_China ? (g_Big5 ? "籓縊9" : "台灯9") : "Table Lamp9")
#define ESID_Jd_TaiDeng10                (g_China ? (g_Big5 ? "籓縊10" : "台灯10") : "Table Lamp10")
#define ESID_Jd_LouDiDeng1               (g_China ? (g_Big5 ? "辅縊1" : "落地灯1") : "Floor Lamp1")
#define ESID_Jd_LouDiDeng2               (g_China ? (g_Big5 ? "辅縊2" : "落地灯2") : "Floor Lamp2")
#define ESID_Jd_LouDiDeng3               (g_China ? (g_Big5 ? "辅縊3" : "落地灯3") : "Floor Lamp3")
#define ESID_Jd_LouDiDeng4               (g_China ? (g_Big5 ? "辅縊4" : "落地灯4") : "Floor Lamp4")
#define ESID_Jd_LouDiDeng5               (g_China ? (g_Big5 ? "辅縊5" : "落地灯5") : "Floor Lamp5")
#define ESID_Jd_LouDiDeng6               (g_China ? (g_Big5 ? "辅縊6" : "落地灯6") : "Floor Lamp6")
#define ESID_Jd_LouDiDeng7               (g_China ? (g_Big5 ? "辅縊7" : "落地灯7") : "Floor Lamp7")
#define ESID_Jd_LouDiDeng8               (g_China ? (g_Big5 ? "辅縊8" : "落地灯8") : "Floor Lamp8")
#define ESID_Jd_LouDiDeng9               (g_China ? (g_Big5 ? "辅縊9" : "落地灯9") : "Floor Lamp9")
#define ESID_Jd_LouDiDeng10              (g_China ? (g_Big5 ? "辅縊10" : "落地灯10") : "Floor Lamp10")
#define ESID_Jd_ZhaoMingDeng1            (g_China ? (g_Big5 ? "酚縊1" : "照明灯1") : "Lighting Lamp1")
#define ESID_Jd_ZhaoMingDeng2            (g_China ? (g_Big5 ? "酚縊2" : "照明灯2") : "Lighting Lamp2")
#define ESID_Jd_ZhaoMingDeng3            (g_China ? (g_Big5 ? "酚縊3" : "照明灯3") : "Lighting Lamp3")
#define ESID_Jd_ZhaoMingDeng4            (g_China ? (g_Big5 ? "酚縊4" : "照明灯4") : "Lighting Lamp4")
#define ESID_Jd_ZhaoMingDeng5            (g_China ? (g_Big5 ? "酚縊5" : "照明灯5") : "Lighting Lamp5")
#define ESID_Jd_ZhaoMingDeng6            (g_China ? (g_Big5 ? "酚縊6" : "照明灯6") : "Lighting Lamp6")
#define ESID_Jd_ZhaoMingDeng7            (g_China ? (g_Big5 ? "酚縊7" : "照明灯7") : "Lighting Lamp7")
#define ESID_Jd_ZhaoMingDeng8            (g_China ? (g_Big5 ? "酚縊8" : "照明灯8") : "Lighting Lamp8")
#define ESID_Jd_ZhaoMingDeng9            (g_China ? (g_Big5 ? "酚縊9" : "照明灯9") : "Lighting Lamp9")
#define ESID_Jd_ZhaoMingDeng10           (g_China ? (g_Big5 ? "酚縊10" : "照明灯10") : "Lighting Lamp10")
#define ESID_Jd_ShiYingDeng1             (g_China ? (g_Big5 ? "ホ璣縊1" : "石英灯1") : "Quartz Lamp1")
#define ESID_Jd_ShiYingDeng2             (g_China ? (g_Big5 ? "ホ璣縊2" : "石英灯2") : "Quartz Lamp2")
#define ESID_Jd_ShiYingDeng3             (g_China ? (g_Big5 ? "ホ璣縊3" : "石英灯3") : "Quartz Lamp3")
#define ESID_Jd_ShiYingDeng4             (g_China ? (g_Big5 ? "ホ璣縊4" : "石英灯4") : "Quartz Lamp4")
#define ESID_Jd_ShiYingDeng5             (g_China ? (g_Big5 ? "ホ璣縊5" : "石英灯5") : "Quartz Lamp5")
#define ESID_Jd_ShiYingDeng6             (g_China ? (g_Big5 ? "ホ璣縊6" : "石英灯6") : "Quartz Lamp6")
#define ESID_Jd_ShiYingDeng7             (g_China ? (g_Big5 ? "ホ璣縊7" : "石英灯7") : "Quartz Lamp7")
#define ESID_Jd_ShiYingDeng8             (g_China ? (g_Big5 ? "ホ璣縊8" : "石英灯8") : "Quartz Lamp8")
#define ESID_Jd_ShiYingDeng9             (g_China ? (g_Big5 ? "ホ璣縊9" : "石英灯9") : "Quartz Lamp9")
#define ESID_Jd_ShiYingDeng10            (g_China ? (g_Big5 ? "ホ璣縊10" : "石英灯10") : "Quartz Lamp10")
#define ESID_Jd_YeDeng1                  (g_China ? (g_Big5 ? "縊1" : "夜灯1") : "Nightlights1")
#define ESID_Jd_YeDeng2                  (g_China ? (g_Big5 ? "縊2" : "夜灯2") : "Nightlights2")
#define ESID_Jd_YeDeng3                  (g_China ? (g_Big5 ? "縊3" : "夜灯3") : "Nightlights3")
#define ESID_Jd_YeDeng4                  (g_China ? (g_Big5 ? "縊4" : "夜灯4") : "Nightlights4")
#define ESID_Jd_YeDeng5                  (g_China ? (g_Big5 ? "縊5" : "夜灯5") : "Nightlights5")
#define ESID_Jd_YeDeng6                  (g_China ? (g_Big5 ? "縊6" : "夜灯6") : "Nightlights6")
#define ESID_Jd_YeDeng7                  (g_China ? (g_Big5 ? "縊7" : "夜灯7") : "Nightlights7")
#define ESID_Jd_YeDeng8                  (g_China ? (g_Big5 ? "縊8" : "夜灯8") : "Nightlights8")
#define ESID_Jd_YeDeng9                  (g_China ? (g_Big5 ? "縊9" : "夜灯9") : "Nightlights9")
#define ESID_Jd_YeDeng10                 (g_China ? (g_Big5 ? "縊10" : "夜灯10") : "Nightlights10")
#define ESID_Jd_ZhuanshiDEng1            (g_China ? (g_Big5 ? "杆耿縊1" : "装饰灯1") : "Deco Lamp1")
#define ESID_Jd_ZhuanshiDEng2            (g_China ? (g_Big5 ? "杆耿縊2" : "装饰灯2") : "Deco Lamp2")
#define ESID_Jd_ZhuanshiDEng3            (g_China ? (g_Big5 ? "杆耿縊3" : "装饰灯3") : "Deco Lamp3")
#define ESID_Jd_ZhuanshiDEng4            (g_China ? (g_Big5 ? "杆耿縊4" : "装饰灯4") : "Deco Lamp4")
#define ESID_Jd_ZhuanshiDEng5            (g_China ? (g_Big5 ? "杆耿縊5" : "装饰灯5") : "Deco Lamp5")
#define ESID_Jd_ZhuanshiDEng6            (g_China ? (g_Big5 ? "杆耿縊6" : "装饰灯6") : "Deco Lamp6")
#define ESID_Jd_ZhuanshiDEng7            (g_China ? (g_Big5 ? "杆耿縊7" : "装饰灯7") : "Deco Lamp7")
#define ESID_Jd_ZhuanshiDEng8            (g_China ? (g_Big5 ? "杆耿縊8" : "装饰灯8") : "Deco Lamp8")
#define ESID_Jd_ZhuanshiDEng9            (g_China ? (g_Big5 ? "杆耿縊9" : "装饰灯9") : "Deco Lamp9")
#define ESID_Jd_ZhuanshiDEng10           (g_China ? (g_Big5 ? "杆耿縊10" : "装饰灯10") : "Deco Lamp10")
#ifdef _AIR_VOLUME_MODE_
#define ESID_Jd_Air_Volume            	 (g_China ? (g_Big5 ? "秖" : "风量") : "Air")
#define ESID_Jd_Low_Wind                 (g_China ? (g_Big5 ? "" : "低风") : "Low")
#define ESID_Jd_Medium_Wind              (g_China ? (g_Big5 ? "い" : "中风") : "Medium")
#define ESID_Jd_Hight_Wind               (g_China ? (g_Big5 ? "蔼" : "高风") : "Hight")
#define ESID_Jd_Automatic                (g_China ? (g_Big5 ? "笆" : "自动") : "Automatic")
#endif

// 设置类
#define ESID_Set_Menu_System             (g_China ? (g_Big5 ? "╰参砞竚" : "系统设置") : "System Settings")
#define ESID_Set_Menu_Project            (g_China ? (g_Big5 ? "祘砞竚" : "工程设置") : "Installer Settings")
#define ESID_Set_Menu_Alarm              (g_China ? (g_Big5 ? "ň砞竚" : "安防设置") : "Security Settings")
#define ESID_Set_Menu_User               (g_China ? (g_Big5 ? "ノめ砞竚" : "用户设置") : "User Settings")
#define ESID_Set_System_language         (g_China ? (g_Big5 ? "╰参粂ē" : "系统语言") : "Language")
#define ESID_Set_System_Time             (g_China ? (g_Big5 ? "ら戳籔丁" : "日期与时间") : "Time & Date")
#define ESID_Set_System_Desk             (g_China ? (g_Big5 ? "璉春" : "桌面背景") : "Desktop")
#define ESID_Set_System_Screen           (g_China ? (g_Big5 ? "棵辊玂臔" : "屏幕保护") : "Screen Saver")
#define ESID_Set_System_Light            (g_China ? (g_Big5 ? "棵辊獹" : "屏幕亮度") : "Screen Brightness")
#define ESID_Set_System_Storage          (g_China ? (g_Big5 ? "纗恨瞶" : "存储管理") : "Memory Management")
#define ESID_Set_System_Info             (g_China ? (g_Big5 ? "╰参癟" : "系统信息") : "System Info.")
#define ESID_Set_Prj_Pass                (g_China ? (g_Big5 ? "祘盞絏砞竚" : "工程密码设置") : "Installer Password")
#define ESID_Set_Prj_Alarm               (g_China ? (g_Big5 ? "ň跋妮┦" : "防区属性") : "Defense Zones")
#define ESID_Set_Prj_Net                 (g_China ? (g_Big5 ? "呼隔把计" : "网络参数") : "Network Parameters")
#define ESID_Set_Prj_ExtMode             (g_China ? (g_Big5 ? "场家舱" : "外部模块") : "External Modules")
#define ESID_Set_Prj_DevNo               (g_China ? (g_Big5 ? "砞称絪腹" : "设备编号") : "Device No.")
#define ESID_Set_Prj_NetDoor             (g_China ? (g_Big5 ? "呼蹈玡诀" : "网络门前机") : "IP Camera")
#define ESID_Set_Prj_Default             (g_China ? (g_Big5 ? "確紅砞竚" : "恢复出厂设置") : "Restore Factory Settings")
#define ESID_Set_Prj_JiaDian             (g_China ? (g_Big5 ? "產筿砞竚" : "家电设置") : "Home Appliances")
#define ESID_Set_Prj_Mic                 (g_China ? (g_Big5 ? "珆繷糤痲砞竚" : "咪头增益设置") : "Mic Gain Settings")
#define ESID_Set_Prj_Mic_Db              (g_China ? (g_Big5 ? "珆繷糤痲" : "咪头增益") : "Mic Gain")
#define ESID_Set_Prj_Set_Rtsp            (g_China ? (g_Big5 ? "菏北砞竚" : "监控设置") : "Monitor Settings")
#define ESID_Set_Prj_Set_IPModule        (g_China ? (g_Big5 ? "IP﹚锣传竟" : "IP协议转换器") : "IP Converter")
#define ESID_Set_Alarm_UserPwd           (g_China ? (g_Big5 ? "ノめ盞絏" : "用户密码") : "User Password")
#define ESID_Set_Alarm_DoorPwd           (g_China ? (g_Big5 ? "秨盞絏" : "开门密码") : "Unlock Password")
#define ESID_Set_Alarm_AfLink            (g_China ? (g_Big5 ? "ň羛笆" : "安防联动") : "Security Interlocking")
#define ESID_Set_Alarm_AfGeLi            (g_China ? (g_Big5 ? "ň跋筳瞒" : "防区隔离") : "Bypass")
#define ESID_Set_Alarm_AfJuFang          (g_China ? (g_Big5 ? "ЫňΤ" : "局防有效") : "Night Mode")
#define ESID_Set_UAlarm_Remote           (g_China ? (g_Big5 ? "环祘北" : "远程控制") : "Remote Control")
#define ESID_Set_Alarm_MsgNum            (g_China ? (g_Big5 ? "祏獺腹絏砞竚" : "短信号码设置") : "Alarm MSG No.")
#define ESID_Set_Alarm_TelNum            (g_China ? (g_Big5 ? "厨牡腹絏砞竚" : "报警号码设置") : "Alarm Call No.")
#define ESID_Set_User_Sound              (g_China ? (g_Big5 ? "筧羘砞竚" : "铃声设置") : "Ringtone")
#define ESID_Set_User_Volume             (g_China ? (g_Big5 ? "秖砞竚" : "音量设置") : "Volume")
#define ESID_Set_User_NoFace             (g_China ? (g_Big5 ? "ゴ耑砞竚" : "免打扰设置") : "Do-Not-Disturb")
#define ESID_Set_User_LyLy               (g_China ? (g_Big5 ? "痙紇痙ē砞竚" : "留影留言设置") : "MSG")
#define ESID_Set_User_PT                 (g_China ? (g_Big5 ? "キ籓爹" : "平台注册") : "Registration")
#define ESID_Set_Unreg                   (g_China ? (g_Big5 ? "キ籓ゼ爹" : "平台未注册") : "Device not registered on Aurine Platform yet!")
#define ESID_Set_Reg_Code                (g_China ? (g_Big5 ? "キ籓喷靡絏:" : "平台验证码:") : "Verification Code:")
#define ESID_Set_Reging                  (g_China ? (g_Big5 ? "タ秈︽キ籓爹,叫祔..." : "正在进行平台注册,请稍候...") : "Registering...")
#define ESID_Set_Prj_Net_HostIP          (g_China ? (g_Big5 ? "セ诀IP" : "本机IP") : "Device IP")
#define ESID_Set_Prj_Net_HostNetMask     (g_China ? (g_Big5 ? "セ诀呼隔綛竛" : "本机子网掩码") : "Subnet Mask")    
#define ESID_Set_Prj_Net_HostGateWay     (g_China ? (g_Big5 ? "セ诀呼隔笵" : "本机网关") : "Gateway") 
#define ESID_Set_Prj_Net_CenterSever     (g_China ? (g_Big5 ? "いみ狝竟IP" : "中心服务器IP") : "Center Server")
#define ESID_Set_Prj_Net_ManageIP1       (g_China ? (g_Big5 ? "恨瞶诀IP" : "管理员机一IP") : "Admin Unit1")  
#define ESID_Set_Prj_Net_ManageIP2       (g_China ? (g_Big5 ? "恨瞶诀IP" : "管理员机二IP") : "Admin Unit2") 
#define ESID_Set_Prj_Net_ManageIP3       (g_China ? (g_Big5 ? "恨瞶诀IP" : "管理员机三IP") : "Admin Unit3")
#define ESID_Set_Prj_Net_AurineSever     (g_China ? (g_Big5 ? "キ籓狝竟IP" : "平台服务器IP") : "Platform Server")  
#define ESID_Set_Prj_Net_AurineElevator  (g_China ? (g_Big5 ? "筿辫北竟IP" : "电梯控制器IP") : "Elevator Controller") 
#define ESID_Set_Prj_Net_Rtsp            (g_China ? (g_Big5 ? "瑈碈砰狝竟" : "流媒体服务器") : "RTSP Server") 
#define ESID_Set_Prj_Net_Stair           (g_China ? (g_Big5 ? "冻狠钡竟IP" : "云端接入器IP") : "Cloud Access Device")
#define ESID_Set_Prj_Net_IPModule_IP     (g_China ? (g_Big5 ? "IP﹚锣传竟" : "IP协议转换器地址") : "IP Address")    
#define ESID_Set_Prj_Net_IPModule_Code   (g_China ? (g_Big5 ? "IP﹚锣传竟竕﹚絏" : "IP协议转换器绑定码") : "Binding Code")      
#define ESID_Set_Dev_Rule                (g_China ? (g_Big5 ? "絪腹砏玥" : "编号规则") : "Number Rule")
#define ESID_Set_Dev_Stair_len           (g_China ? (g_Big5 ? "辫腹" : "梯号长度") : "Stairway No.")
#define ESID_Set_Dev_Room_len            (g_China ? (g_Big5 ? "┬腹" : "房号长度") : "Apartment No.")
#define ESID_Set_Dev_Cell_len            (g_China ? (g_Big5 ? "虫じ腹" : "单元号长度") : "Riser No.")
#define ESID_Set_Dev_UseCell             (g_China ? (g_Big5 ? "币ノ虫じ腹" : "启用单元号") : "Enable Riser No.")
#define ESID_Set_Dev_BIT                 (g_China ? (g_Big5 ? "" : "位") : "Digits")
#define ESID_Set_Ext_Alarm               (g_China ? (g_Big5 ? "ň家舱" : "安防模块") : "Alarm Module")
#define ESID_Set_Ext_JiaDian             (g_China ? (g_Big5 ? "產筿家舱" : "家电模块") : "Home Automation Module")
#define ESID_Set_Ext_Stair               (g_China ? (g_Big5 ? "硄ノ辫诀" : "通用梯口机") : "General Secondary Entry Station")
#define ESID_Set_Language                (g_China ? (g_Big5 ? "叫絋粄琌玂砞竚?" : "请确认是否保存设置?") : "Save The Setting?")
#ifdef _TCPTO485_ELEVATOR_MODE_
#define ESID_Set_TCP485Elevator           (g_China ? (g_Big5 ? "TCP锣485家舱IP" : "TCP转485模块IP") : "TCP To 485 Addr")
#endif

// IP协议转换器
#define ESID_Set_binding_code        	 (g_China ? (g_Big5 ? "竕﹚絏:" : "绑定码:") : "Binding Code:")
#define ESID_Set_binding_code1        	 (g_China ? (g_Big5 ? "竕﹚絏" : "绑定码") : "Binding Code") 
#define ESID_Set_binding_status          (g_China ? (g_Big5 ? "竕﹚篈:" : "绑定状态:") : "Binding State:")
#define ESID_Set_extension_information   (g_China ? (g_Big5 ? "だ诀獺:" : "分机信息:") : "EXT-INFO:")
#define ESID_Set_ip_ptcAddr				 (g_China ? (g_Big5 ? "﹚锣传竟IP:" : "协议转换器IP:") : "IP Address:")
#define ESID_Set_generate_binding_code   (g_China ? (g_Big5 ? "ネΘ竕﹚絏" : "生成绑定码") : "Binding Code")
#define ESID_Set_binding  				 (g_China ? (g_Big5 ? "竕﹚" : "绑定") : "Binding")
#define ESID_Set_get_extension			 (g_China ? (g_Big5 ? "莉だ诀" : "获取分机") : "Extension")
#define ESID_Set_bounded  				 (g_China ? (g_Big5 ? "竕﹚" : "已绑定") : "Binding")
#define ESID_Set_unbounded				 (g_China ? (g_Big5 ? "ゼ竕﹚" : "未绑定") : "Unbounded")
#define ESID_Set_ExtenNo 				 (g_China ? (g_Big5 ? "だ诀" : "分机") : "EXT")
#define ESID_Set_ExtenState				 (g_China ? (g_Big5 ? "篈" : "状态") : "Status")
#define ESID_Set_Online				     (g_China ? (g_Big5 ? "絬" : "在线") : "OnLine")
#define ESID_Set_Offline 				 (g_China ? (g_Big5 ? "瞒絬" : "离线") : "UnLine")
#define ESID_Set_Bind_OK				 (g_China ? (g_Big5 ? "竕﹚Θ\" : "绑定成功") : "OK")
#define ESID_Set_Bind_Fail				 (g_China ? (g_Big5 ? "竕﹚ア毖" : "绑定失败") : "Failed")

// 网络门前机文字
#define ESID_Net_Door_First_R            (g_China ? (g_Big5 ? "玡诀" : "门前机一") : "Camera 1")
#define ESID_Net_Door_Second_R           (g_China ? (g_Big5 ? "玡诀" : "门前机二") : "Camera 2")
#define ESID_Net_Door_First_A            (g_China ? (g_Big5 ? "家览玡诀" : "模拟门前机一") : "Analog Camera 1")
#define ESID_Net_Door_Second_A           (g_China ? (g_Big5 ? "家览玡诀" : "模拟门前机二") : "Analog Camera 2")
#define ESID_Net_Door             		 (g_China ? (g_Big5 ? "呼蹈玡诀" : "网络门前机") : "IP Camera")
#define ESID_Net_Door_First              (g_China ? (g_Big5 ? "呼蹈玡诀" : "网络门前机一") : "IP Camera 1")
#define ESID_Net_Door_Second             (g_China ? (g_Big5 ? "呼蹈玡诀" : "网络门前机二") : "IP Camera 2")
#define ESID_Net_Door_IP                 (g_China ? (g_Big5 ? "IP" : "IP地址") : "IP Address")
#define ESID_Net_Door_Sub                (g_China ? (g_Big5 ? "呼隔綛竛" : "子网掩码") : "Subnet Mask")
#define ESID_Net_Door_GateWay            (g_China ? (g_Big5 ? "呼隔笵" : "网关") : "Gateway")
#define ESID_Net_Door_GetMAC             (g_China ? (g_Big5 ? "弄MAC絏" : "读MAC码") : "Obtain MAC")
#define ESID_Net_Door_MAC                (g_China ? (g_Big5 ? "MAC" : "MAC") : "MAC")
#define ESID_Net_Door_NetParam           (g_China ? (g_Big5 ? "呼隔把计" : "网络参数") : "Network Parameters")
#define ESID_Net_Door_LockType           (g_China ? (g_Big5 ? "玛摸砞竚" : "锁类型设置") : "Lock")
#define ESID_Net_Door_CardMan            (g_China ? (g_Big5 ? "恨瞶" : "卡管理") : "IC Card")
#define ESID_Net_Door_SysInfo            (g_China ? (g_Big5 ? "╰参癟" : "系统信息") : "System Info.")
#define ESID_Net_Door_Other_Set          (g_China ? (g_Big5 ? "ㄤ砞竚" : "其它设置") : "Other Settings")
#define ESID_Net_Door_Open               (g_China ? (g_Big5 ? "盽秨" : "常开") : "N/O")
#define ESID_Net_Door_Close              (g_China ? (g_Big5 ? "盽超" : "常闭") : "N/C")
#define ESID_Net_Door_LTime_Mh           (g_China ? (g_Big5 ? "秨玛" : "开锁时长:") : "Unlock Time:")
#define ESID_Net_Door_LockTime           (g_China ? (g_Big5 ? "秨玛" : "开锁时长") : "Unlock Time")
#define ESID_Net_Door_Pic_Up             (g_China ? (g_Big5 ? "╃酚肚" : "拍照上传") : "Snapshot Upload")
#define ESID_Net_Door_Remote_Moniter     (g_China ? (g_Big5 ? "环祘菏跌" : "远程监视") : "Remote Monitoring")
#define ESID_Net_Door_Add_Card           (g_China ? (g_Big5 ? "糤" : "增加卡") : "Add Card")
#define ESID_Net_Door_Del_Card           (g_China ? (g_Big5 ? "埃" : "删除卡") : "Del Card")
#define ESID_Net_Door_Unused_First       (g_China ? (g_Big5 ? "ゼ币ノ呼蹈玡诀!" : "未启用网络门前机一!") : "IP Camera 1 Inactivated!")
#define ESID_Net_Door_Unused_Second      (g_China ? (g_Big5 ? "ゼ币ノ呼蹈玡诀!" : "未启用网络门前机二!") : "IP Camera 2 Inactivated!")
#define ESID_Net_Door_Pls_Add_Card       (g_China ? (g_Big5 ? "叫眤┮惠璶糤!" : "请刷您所需要增加的卡!") : "Please add the card!")
#define ESID_Net_Door_Sure_Del_Card      (g_China ? (g_Big5 ? "琌埃┮Τ?" : "是否删除所有的卡?") : "Sure to delete all cards?")
#define ESID_Net_Door_Video_Fmt          (g_China ? (g_Big5 ? "炊硄跌繵借秖" : "普通视频质量") : "Standard Video Quality")
#define ESID_Set_Info_HW                 (g_China ? (g_Big5 ? "祑砰セ" : "硬件版本") : "Hardware Ver")
#define ESID_Set_Info_SF                 (g_China ? (g_Big5 ? "硁砰セ" : "软件版本") : "Software Ver")
#define ESID_Set_Info_SDK                (g_China ? (g_Big5 ? "SDKセ" : "SDK版本") : "SDK Ver")
#define ESID_Set_Info_HostMAC            (g_China ? (g_Big5 ? "セ诀MAC" : "本机MAC") : "MAC")
#define ESID_Set_Info_Room               (g_China ? (g_Big5 ? "セ诀┬腹" : "本机房号") : "Device No.")
#define ESID_Set_Timer_Wanl              (g_China ? (g_Big5 ? "窾句" : "万年历") : "Calendar")
#define ESID_Storage_Format              (g_China ? (g_Big5 ? "Αて" : "格式化") : "Format")
#define ESID_Storage_Pic                 (g_China ? (g_Big5 ? "瓜" : "图片") : "Picture")
#define ESID_Set_Pass_Server             (g_China ? (g_Big5 ? "恨產盞絏" : "家政密码") : "Servant Password")
#define ESID_Set_Pass_DoorUser           (g_China ? (g_Big5 ? "め秨盞絏" : "住户开门密码") : "Household Password")
#define ESID_Set_Pass_DoorServer         (g_China ? (g_Big5 ? "恨產秨盞絏" : "家政开门密码") : "Servant Password")
#define ESID_Set_Pass_Old                (g_China ? (g_Big5 ? "侣盞絏" : "旧密码") : "Old:")
#define ESID_Set_Pass_New                (g_China ? (g_Big5 ? "穝盞絏" : "新密码") : "New:")
#define ESID_Set_Pass_Query              (g_China ? (g_Big5 ? "絋粄盞絏" : "确认密码") : "Re-enter:")
#define ESID_Set_volume_ring             (g_China ? (g_Big5 ? "筧羘秖" : "铃声音量") : "Ringing Volume")
#define ESID_Set_volume_talk             (g_China ? (g_Big5 ? "硄杠秖" : "通话音量") : "Talk Volume")
#define ESID_Set_volume_key              (g_China ? (g_Big5 ? "币ノ龄" : "启用按键音") : "Keypad Tone")
#define ESID_Set_ring_Stair              (g_China ? (g_Big5 ? "辫ㄓ筿筧羘" : "梯口来电铃声") : "Sec.Entry")
#define ESID_Set_ring_Area               (g_China ? (g_Big5 ? "跋ㄓ筿筧羘" : "区口来电铃声") : "Prim.Entry")
#define ESID_Set_ring_Door               (g_China ? (g_Big5 ? "玡ㄓ筿筧羘" : "门前来电铃声") : "Doorway")
#define ESID_Set_ring_Center             (g_China ? (g_Big5 ? "いみㄓ筿筧羘" : "中心来电铃声") : "Admin")
#define ESID_Set_ring_Room               (g_China ? (g_Big5 ? "めㄓ筿筧羘" : "住户来电铃声") : "Resident")
#define ESID_Set_ring_fenji              (g_China ? (g_Big5 ? "だ诀ㄓ筿筧羘" : "分机来电铃声") : "Extension")
#define ESID_Set_ring_Info               (g_China ? (g_Big5 ? "癟矗ボ" : "信息提示音") : "New Message")
#define ESID_Set_ring_alarm              (g_China ? (g_Big5 ? "箇牡矗ボ" : "预警提示音") : "Exit Delay Alert")
#define ESID_Set_ring                    (g_China ? (g_Big5 ? "筧羘" : "铃声") : "Ringtone")
#define ESID_Set_hit                     (g_China ? (g_Big5 ? "矗ボ" : "提示音") : "Tip Sound")
#define ESID_Set_Lyly_Mode_Audio         (g_China ? (g_Big5 ? "痙ē家Α" : "纯留言模式") : "Voice MSG")
#define ESID_Set_Lyly_Mode_Both          (g_China ? (g_Big5 ? "痙ē酚家Α" : "留言照片模式") : "Voice MSG & Image")
#define ESID_Set_Lyly_Mode_Video         (g_China ? (g_Big5 ? "痙紇痙ē家Α" : "留影留言模式") : "Video MSG")
#define ESID_Set_Lyly_Link1              (g_China ? (g_Big5 ? "篈羛笆痙紇痙ē" : "外出状态下联动留影留言") : "Activate MSG under the AWAY mode")
#define ESID_Set_Lyly_Link2              (g_China ? (g_Big5 ? "丁篈羛笆痙紇痙ē" : "夜间状态下联动留影留言") : "Activate MSG under the NIGHT mode")
#define ESID_Set_Lyly_Link3              (g_China ? (g_Big5 ? "ゴ耑篈羛笆痙紇痙ē" : "免打扰状态下联动留影留言") : "Enable MSG under the Do-Not-Disturb mode")
#define ESID_Set_Lyly_default_tip        (g_China ? (g_Big5 ? "箇砞矗ボ" : "默认提示音") : "Default")
#define ESID_Set_Lyly_record_tip         (g_China ? (g_Big5 ? "魁籹矗ボ" : "录制提示音") : "Customized")
#define ESID_Set_Screen_InTime           (g_China ? (g_Big5 ? "秈棵辊玂臔丁" : "进入屏保时间") : "Starts In")
#define ESID_Set_Screen_Lcd              (g_China ? (g_Big5 ? "闽丁" : "关屏时间") : "Turn off Display")
#define ESID_Set_Query_Del_All        	 (g_China ? (g_Big5 ? "絋﹚琌睲?" : "确定是否清空?") : "Sure to delete all?")
#define ESID_Set_Tips_Fail				 (g_China ? (g_Big5 ? "ゼ魁籹矗ボ" : "未录制提示音") : "No Recording Tips")	

// 家电设置文字
#define ESID_Jd_Set_Light                (g_China ? (g_Big5 ? "縊砞竚" : "灯光设置") : "Light")
#define ESID_Jd_Set_Window               (g_China ? (g_Big5 ? "怠卯砞竚" : "窗帘设置") : "Curtain")
#define ESID_Jd_Set_Kongtiao             (g_China ? (g_Big5 ? "秸砞竚" : "空调设置") : "A/C")
#define ESID_Jd_Set_Dianyuan             (g_China ? (g_Big5 ? "筿方砞竚" : "电源设置") : "Socket")
#define ESID_Jd_Set_Gas                  (g_China ? (g_Big5 ? "费砞竚" : "煤气设置") : "Gas")
#define ESID_Jd_Set_Adrress              (g_China ? (g_Big5 ? "" : "地址") : "Address")
#define ESID_Jd_Set_OpenAddr             (g_China ? (g_Big5 ? "秨" : "开地址") : "Starting Address")
#define ESID_Jd_Set_CloseAddr            (g_China ? (g_Big5 ? "闽" : "关地址") : "Closing Address")
#define ESID_Jd_Set_Dev_Name1            (g_China ? (g_Big5 ? "砞称竚" : "设备位置") : "Address")
#define ESID_Jd_Set_Dev_Name2            (g_China ? (g_Big5 ? "砞称摸" : "设备类型") : "Type")
#define ESID_Jd_Open_All                 (g_China ? (g_Big5 ? "秨" : "全开") : "ALL ON")
#define ESID_Jd_Close_All                (g_China ? (g_Big5 ? "闽" : "全关") : "ALL OFF")
#define ESID_Jd_Qingjing                 (g_China ? (g_Big5 ? "薄春家Α" : "情景模式") : "Scene Mode")
#define ESID_Jd_Light                    (g_China ? (g_Big5 ? "縊" : "灯光") : "Light")
#define ESID_Jd_KongTiao                 (g_China ? (g_Big5 ? "秸" : "空调") : "A/C")
#define ESID_Jd_Dianyuan                 (g_China ? (g_Big5 ? "筿方" : "电源") : "Socket")
#define ESID_Jd_Meiqi                    (g_China ? (g_Big5 ? "费" : "煤气") : "Gas")
#define ESID_Jd_Control_Mode             (g_China ? (g_Big5 ? "北よΑ" : "控制方式") : "Control")
#define ESID_Jd_Signal_Control           (g_China ? (g_Big5 ? "虫北" : "单控") : "Single")
#define ESID_Jd_Multi_Control            (g_China ? (g_Big5 ? "舱北" : "组控") : "Group")
#define ESID_Jd_Group_No                 (g_China ? (g_Big5 ? "舱腹" : "组号") : "Group No.")
#define ESID_Jd_Dev_Set_Attr             (g_China ? (g_Big5 ? "妮┦" : "属性") : "Property")
#define ESID_Jd_Dev_Set_Adjust           (g_China ? (g_Big5 ? "秸" : "可调") : "Adjustable")
#define ESID_Jd_Dev_Set_UnAdjust         (g_China ? (g_Big5 ? "ぃ秸" : "不可调") : "Unadjustable")
#define ESID_Jd_Dev_Set_Light            (g_China ? (g_Big5 ? "獹" : "亮度") : "Brightness")
#define ESID_Jd_Dev_Set_Temp             (g_China ? (g_Big5 ? "放" : "温度") : "Temperature")
#define ESID_Jd_Dev_Set_NoDev            (g_China ? (g_Big5 ? "礚砞称" : "无设备") : "None")
#define ESID_Jd_Dev_Name                 (g_China ? (g_Big5 ? "嘿" : "名称") : "Description")
#define ESID_Jd_KongTiao_Learn           (g_China ? (g_Big5 ? "厩策" : "红外学习") : "IR Study")
#define ESID_Jd_KongTiao_Cold            (g_China ? (g_Big5 ? "籹厩策" : "制冷学习") : "Cool")
#define ESID_Jd_KongTiao_Hot             (g_China ? (g_Big5 ? "穢厩策" : "制暖学习") : "Heat")
#define ESID_Jd_KongTiao_Mode            (g_China ? (g_Big5 ? "家Α厩策" : "模式学习") : "Mode")
#define ESID_Jd_Learn                    (g_China ? (g_Big5 ? "秨﹍厩策" : "开始学习") : "Start")
#define ESID_Jd_Test                     (g_China ? (g_Big5 ? "代刚" : "测试") : "Test")
#define ESID_Jd_KongTiao_Refre           (g_China ? (g_Big5 ? "睲瞡" : "清爽") : "Fresh")
#define ESID_Jd_KongTiao_Warm            (g_China ? (g_Big5 ? "放穢" : "温暖") : "Warm")
#define ESID_Jd_KongTiao_Leisure         (g_China ? (g_Big5 ? "ヰ秪" : "休闲") : "Casual")
#define ESID_Jd_KongTiao_Sleep           (g_China ? (g_Big5 ? "何痸" : "睡眠") : "Sleep")
#define ESID_Jd_KongTiao_Close           (g_China ? (g_Big5 ? "闽超" : "关闭") : "OFF")
#define ESID_Jd_Button_Cold              (g_China ? (g_Big5 ? "籹" : "制冷") : "Cool")
#define ESID_Jd_Button_Warm              (g_China ? (g_Big5 ? "穢" : "制暖") : "Heat")
#define ESID_Jd_Button_Mode              (g_China ? (g_Big5 ? "家Α" : "模式") : "Mode")
#define ESID_Jd_Window_Stop              (g_China ? (g_Big5 ? "氨ゎ" : "停止") : "Stop")
#define ESID_Jd_Lamp                     (g_China ? (g_Big5 ? "縊" : "灯光") : "Lamp")
#define ESID_Jd_Qingjing1                (g_China ? (g_Big5 ? "薄春家Α" : "情景模式") : "Scene")
#define ESID_Jd_Set_Name_Err             (g_China ? (g_Big5 ? "嘿狡,玂ア毖!" : "名称重复,保存失败!") : "Description Repeated!")
#define ESID_Jd_Set_Adr_Err              (g_China ? (g_Big5 ? "狡,ぃ砞竚!" : "地址重复,不能设置!") : "Address Repeated!")
#define ESID_Jd_Set_OpAdr_Err            (g_China ? (g_Big5 ? "秨狡,ぃ砞竚!" : "开地址重复,不能设置!") : "Same Starting Address")
#define ESID_Jd_Set_ClAdr_Err            (g_China ? (g_Big5 ? "闽狡,ぃ砞竚!" : "关地址重复,不能设置!") : "Same Closing Address")
#define ESID_Jd_Set_Dev_Failed           (g_China ? (g_Big5 ? "砞称砞竚ア毖!" : "设备设置失败!") : "Failed!")
#define ESID_Jd_Set_Dev_Noname           (g_China ? (g_Big5 ? "叫砞竚嘿!" : "请设置名称!") : "Please enter the device name!")
#define ESID_Jd_Adress_Set_Warning       (g_China ? (g_Big5 ? "叫块絛瞅琌0-255" : "请输入范围是0-255的地址") : "Address shall be within 0-255.")
#define ESID_Jd_Set_Scene_Noname         (g_China ? (g_Big5 ? "叫砞竚嘿!" : "请设置名称!") : "Please enter the scene name!")
#define ESID_Jd_Set_KtLearn_Err          (g_China ? (g_Big5 ? "厩策ア毖,叫穝厩策!" : "学习失败,请重新学习!") : "Failed! Pls try it again.")
#define ESID_Jd_Set_KtLearn_Ok           (g_China ? (g_Big5 ? "厩策Θ\!" : "学习成功!") : "OK!")
#define ESID_Jd_Set_Adr_Wrong            (g_China ? (g_Big5 ? "叫块絛瞅琌16-254" : "请输入范围是16-254的地址") : "Address shall be within 16-254.")
#define ESID_Jd_Sence_Adr                (g_China ? (g_Big5 ? "(絛瞅16-254)" : "(地址范围为16-254)") : "(Address shall be within 16-254)")
#define ESID_Jd_Aurine_Adress_Set_Warning  (g_China ? (g_Big5 ? "叫块絛瞅琌0-254" : "请输入范围是0-254的地址") : "Address shall be within 0-254.")
#define ESID_Jd_Kongtiao_Learning        (g_China ? (g_Big5 ? "厩策い,叫祔..." : "学习中,请稍候...") : "Studying, pls wait...")
#define ESID_Jd_Set_Scene                (g_China ? (g_Big5 ? "薄春砞竚" : "情景设置") : "Scene Mode")
#define ESID_Jd_Mode_Huike               (g_China ? (g_Big5 ? "穦家Α" : "会客模式") : "Meeting")
#define ESID_Jd_Mode_Jiucan              (g_China ? (g_Big5 ? "碞繺\家Α" : "就餐模式") : "Dining")
#define ESID_Jd_Mode_Yejian              (g_China ? (g_Big5 ? "丁家Α" : "夜间模式") : "Night")
#define ESID_Jd_Mode_Jiedian             (g_China ? (g_Big5 ? "竊筿家Α" : "节电模式") : "Saving")
#define ESID_Jd_Mode_Putong              (g_China ? (g_Big5 ? "炊硄家Α" : "普通模式") : "Regular")
#define ESID_Jd_Mode_Self                (g_China ? (g_Big5 ? "薄春家Α" : "情景模式") : "Scene Mode")
#define ESID_Jd_Mode_Set_Name1           (g_China ? (g_Big5 ? "薄春" : "情景名字") : "Scene Name")
#define ESID_Jd_Mode_Set_Name2           (g_China ? (g_Big5 ? "薄春摸" : "情景类型") : "Scene Type")
#define ESID_Jd_Mode_Out                 (g_China ? (g_Big5 ? "" : "外出") : "Away")
#define ESID_Jd_Mode_Cook                (g_China ? (g_Big5 ? "瞚豆" : "烹饪") : "Cooking")
#define ESID_Jd_Mode_first               (g_China ? (g_Big5 ? "Ν" : "早茶") : "Breakfast")
#define ESID_Jd_Mode_Afater              (g_China ? (g_Big5 ? "と繺\" : "午餐") : "Lunch")
#define ESID_Jd_Mode_Night               (g_China ? (g_Big5 ? "边産" : "晚宴") : "Supper")
#define ESID_Jd_Mode_Bed                 (g_China ? (g_Big5 ? "碞轨" : "就寝") : "Bed")
#define ESID_Jd_Mode_QiYe                (g_China ? (g_Big5 ? "癬" : "起夜") : "Mid-night")
#define ESID_Jd_Mode_Up                  (g_China ? (g_Big5 ? "癬" : "起床") : "Wake-up")
#define ESID_Jd_Mode_Muyu                (g_China ? (g_Big5 ? "∟疍" : "沐浴") : "Bath")
#define ESID_Jd_Mode_Tea                 (g_China ? (g_Big5 ? "珇" : "品茶") : "Tea")
#define ESID_Jd_Mode_Drink               (g_China ? (g_Big5 ? "都皊" : "饮酒") : "Bibulous")
#define ESID_Jd_Mode_Read                (g_China ? (g_Big5 ? "綷\弄" : "阅读") : "Reading")
#define ESID_Jd_Mode_Work                (g_China ? (g_Big5 ? "" : "工作") : "Working")
#define ESID_Jd_Mode_Juhui               (g_China ? (g_Big5 ? "籈穦" : "聚会") : "Party")
#define ESID_Jd_Mode_Play                (g_China ? (g_Big5 ? "囱礟" : "棋牌") : "Chess&Cards")
#define ESID_Jd_Mode_Film                (g_China ? (g_Big5 ? "紇皘" : "影院") : "Theatre")
#define ESID_Jd_Mode_Sing                (g_China ? (g_Big5 ? "舧佰" : "欢唱") : "Carolling")
#define ESID_Jd_Mode_Storang             (g_China ? (g_Big5 ? "胺ō" : "健身") : "Exercising")
#define ESID_Jd_Mode_Wenxi               (g_China ? (g_Big5 ? "放纳" : "温馨") : "Cozy")
#define ESID_Jd_Mode_Langman             (g_China ? (g_Big5 ? "憨" : "浪漫") : "Romatic")
#define ESID_Jd_Mode_Jiqing              (g_China ? (g_Big5 ? "縀薄" : "激情") : "Passionate")
#define ESID_Jd_Mode_Dianya              (g_China ? (g_Big5 ? "ㄥ懂" : "典雅") : "Elegant")
#define ESID_Jd_Mode_Chlid               (g_China ? (g_Big5 ? "担届" : "童趣") : "Childish")
#define ESID_Jd_Mode_Chenwen             (g_China ? (g_Big5 ? "↖铆" : "沉稳") : "Sedate")
#define ESID_Jd_Mode_Think               (g_China ? (g_Big5 ? "繰" : "静思") : "Calm")
#define ESID_Jd_Mode_Cool                (g_China ? (g_Big5 ? "睲睤" : "清凉") : "Cool")
#define ESID_Jd_Mode_Warm                (g_China ? (g_Big5 ? "放穢" : "温暖") : "Warm")
#define ESID_Jd_Mode_Mode                (g_China ? (g_Big5 ? "家Α" : "模式") : "Mode")
#define ESID_Jd_Mode_Sence               (g_China ? (g_Big5 ? "薄春" : "情景") : "Scene")
#define ESID_Jd_Mode_Qingdiao            (g_China ? (g_Big5 ? "薄秸" : "情调") : "Sentiment")
#define ESID_Jd_Mode_Fenwei              (g_China ? (g_Big5 ? "猑瞅" : "氛围") : "Atmosphere")
#define ESID_Jd_Mode_Time                (g_China ? (g_Big5 ? "" : "时光") : "Time")
#define ESID_Jd_Mode_Changjing           (g_China ? (g_Big5 ? "初春" : "场景") : "Occasion")
#define ESID_Jd_LouShang                 (g_China ? (g_Big5 ? "加" : "楼上") : "Upstairs")
#define ESID_Jd_LouXia                   (g_China ? (g_Big5 ? "加" : "楼下") : "Downstairs")
#define ESID_Jd_WuNei                    (g_China ? (g_Big5 ? "ず" : "屋内") : "Indoor")
#define ESID_Jd_WuWai                    (g_China ? (g_Big5 ? "" : "屋外") : "Outdoor")
#define ESID_Jd_TianTai                  (g_China ? (g_Big5 ? "ぱ籓" : "天台") : "Roof")
#define ESID_Jd_GeLou                    (g_China ? (g_Big5 ? "徽加" : "阁楼") : "Attic")
#define ESID_Jd_HuaYuan                  (g_China ? (g_Big5 ? "堕" : "花园") : "Garden")
#define ESID_Jd_DiJiao                   (g_China ? (g_Big5 ? "耽" : "地窖") : "Cellar")
#define ESID_Jd_TianJin                  (g_China ? (g_Big5 ? "ぱか" : "天井") : "Patio")
#define ESID_Jd_WuYan                    (g_China ? (g_Big5 ? "姥" : "屋檐") : "Roof")
#define ESID_Jd_QianTing                 (g_China ? (g_Big5 ? "玡畑" : "前庭") : "Vestibule")
#define ESID_Jd_HouYuan                  (g_China ? (g_Big5 ? "皘" : "后院") : "Backyard")
#define ESID_Jd_DianTiJian               (g_China ? (g_Big5 ? "筿辫丁" : "电梯间") : "Elevator")
#define ESID_Jd_TingCheFang              (g_China ? (g_Big5 ? "氨ó┬" : "停车房") : "Parking Room")
#define ESID_Jd_DaMen                    (g_China ? (g_Big5 ? "" : "大门") : "Gate")
#define ESID_Jd_QianMen                  (g_China ? (g_Big5 ? "玡" : "前门") : "Front Door")
#define ESID_Jd_ZhongMen                 (g_China ? (g_Big5 ? "い" : "中门") : "MidDoor")
#define ESID_Jd_PangMen                  (g_China ? (g_Big5 ? "" : "傍门") : "Side Door")
#define ESID_Jd_HouMen                   (g_China ? (g_Big5 ? "" : "后门") : "Backdoor")
#define ESID_Jd_CeMen                    (g_China ? (g_Big5 ? "凹" : "侧门") : "Side gate")
#define ESID_Jd_BianMen                  (g_China ? (g_Big5 ? "娩" : "边门") : "Wicket")
#define ESID_Jd_HenMen                   (g_China ? (g_Big5 ? "绢" : "横门") : "Fence Door")
#define ESID_Jd_YanTai                   (g_China ? (g_Big5 ? "锭籓" : "阳台") : "Balcony")
#define ESID_Jd_Zhoulan                  (g_China ? (g_Big5 ? "ǐ碮" : "走廊") : "Corridor")
#define ESID_Jd_GuoDao                   (g_China ? (g_Big5 ? "筁笵" : "过道") : "Aisle")
#define ESID_Jd_LouTi                    (g_China ? (g_Big5 ? "加辫" : "楼梯") : "Stairs")
#define ESID_Jd_TiKou                    (g_China ? (g_Big5 ? "辫" : "梯口") : "Stairway")
#define ESID_Jd_TiJian                   (g_China ? (g_Big5 ? "辫丁" : "梯间") : "Staircase")
#define ESID_Jd_TiXia                    (g_China ? (g_Big5 ? "辫" : "梯下") : "Down the Ladder")
#define ESID_Jd_KeTing                   (g_China ? (g_Big5 ? "芔" : "客厅") : "Drawing Room")
#define ESID_Jd_DaTing                   (g_China ? (g_Big5 ? "芔" : "大厅") : "Hall")
#define ESID_Jd_ZhuTing                  (g_China ? (g_Big5 ? "芔" : "主厅") : "Main Hall")
#define ESID_Jd_CanTing                  (g_China ? (g_Big5 ? "繺\芔" : "餐厅") : "Restaurants")
#define ESID_Jd_FanTing                  (g_China ? (g_Big5 ? "逗芔" : "饭厅") : "Dining Hall")
#define ESID_Jd_MenTing                  (g_China ? (g_Big5 ? "芔" : "门厅") : "Foyer")
#define ESID_Jd_ZhuWoShi                 (g_China ? (g_Big5 ? "" : "主卧室") : "Master Bedroom")
#define ESID_Jd_ZhuRenFang               (g_China ? (g_Big5 ? "┬" : "主人房") : "Host Room")
#define ESID_Jd_WoShi                    (g_China ? (g_Big5 ? "" : "卧室") : "Bedroom")
#define ESID_Jd_ShuiFang                 (g_China ? (g_Big5 ? "何┬" : "睡房") : "Bedroom")
#define ESID_Jd_FangJianA                (g_China ? (g_Big5 ? "┬丁A" : "房间A") : "RoomA")
#define ESID_Jd_FangJianB                (g_China ? (g_Big5 ? "┬丁B" : "房间B") : "RoomB")
#define ESID_Jd_FangJianC                (g_China ? (g_Big5 ? "┬丁C" : "房间C") : "RoomC")
#define ESID_Jd_FangJianD                (g_China ? (g_Big5 ? "┬丁D" : "房间D") : "RoomD")
#define ESID_Jd_FangJianE                (g_China ? (g_Big5 ? "┬丁E" : "房间E") : "RoomE")
#define ESID_Jd_ChuFang                  (g_China ? (g_Big5 ? "紁┬" : "厨房") : "Kitchen")
#define ESID_Jd_ShuFang                  (g_China ? (g_Big5 ? "┬" : "书房") : "Study")
#define ESID_Jd_KeFang                   (g_China ? (g_Big5 ? "┬" : "客房") : "Rooms")
#define ESID_Jd_ZhuYuShi                 (g_China ? (g_Big5 ? "疍" : "主浴室") : "Master Bathroom")
#define ESID_Jd_YuShi                    (g_China ? (g_Big5 ? "疍" : "浴室") : "Bathroom")
#define ESID_Jd_ErTongFang               (g_China ? (g_Big5 ? "ㄠ担┬" : "儿童房") : "Children Room")
#define ESID_Jd_GongRenFang              (g_China ? (g_Big5 ? "┬" : "工人房") : "Servant Room")
#define ESID_Jd_DuoYongTuJian            (g_China ? (g_Big5 ? "ノ硚丁" : "多用途间") : "Multi-purpose rooms")
#define ESID_Jd_QiJuShi                  (g_China ? (g_Big5 ? "癬﹡" : "起居室") : "Living Room")
#define ESID_Jd_YouLeShi                 (g_China ? (g_Big5 ? "笴贾" : "游乐室") : "Playroom")
#define ESID_Jd_ShuXiShi                 (g_China ? (g_Big5 ? "瑍" : "梳洗室") : "Toilet Room")
#define ESID_Jd_WeiShengJian             (g_China ? (g_Big5 ? "矫ネ丁" : "卫生间") : "Toilet")
#define ESID_Jd_XieMaoJian               (g_China ? (g_Big5 ? "綾碪丁" : "鞋帽间") : "Home Locker Room")
#define ESID_Jd_GongZuoJian              (g_China ? (g_Big5 ? "丁" : "工作间") : "Workplace")
#define ESID_Jd_DiXiaShi                 (g_China ? (g_Big5 ? "" : "地下室") : "Basement")
#define ESID_Jd_None                     (g_China ? (g_Big5 ? "礚" : "无") : "None")
#define ESID_Jd_Deng                     (g_China ? (g_Big5 ? "縊" : "灯") : "Light")
#define ESID_Jd_TiaoGuangDeng            (g_China ? (g_Big5 ? "秸縊" : "调光灯") : "Dimming Lights")
#define ESID_Jd_BaiChiDeng               (g_China ? (g_Big5 ? "フ縆縊" : "白炽灯") : "Incandescent Lamp")
#define ESID_Jd_GuangGuan                (g_China ? (g_Big5 ? "恨" : "光管") : "Lightpipe")
#define ESID_Jd_DiaoDeng                 (g_China ? (g_Big5 ? "縊" : "吊灯") : "Chandelier")
#define ESID_Jd_HuaDeng                  (g_China ? (g_Big5 ? "縊" : "花灯") : "Lantern")
#define ESID_Jd_BiDeng                   (g_China ? (g_Big5 ? "纠縊" : "壁灯") : "Wall Lamp")
#define ESID_Jd_SheDeng                  (g_China ? (g_Big5 ? "甮縊" : "射灯") : "Spotlight")
#define ESID_Jd_TaiDeng                  (g_China ? (g_Big5 ? "籓縊" : "台灯") : "Table Lamp")
#define ESID_Jd_LouDiDeng                (g_China ? (g_Big5 ? "辅縊" : "落地灯") : "Floor Lamp")
#define ESID_Jd_ZhaoMingDeng             (g_China ? (g_Big5 ? "酚縊" : "照明灯") : "Lighting Lamp")
#define ESID_Jd_ShiYingDeng              (g_China ? (g_Big5 ? "ホ璣縊" : "石英灯") : "Quartz Lamp")
#define ESID_Jd_YeDeng                   (g_China ? (g_Big5 ? "縊" : "夜灯") : "Nightlights")
#define ESID_Jd_ZhuanshiDEng             (g_China ? (g_Big5 ? "杆耿縊" : "装饰灯") : "Deco Lamp")
#define ESID_Jd_DianZhiSuo               (g_China ? (g_Big5 ? "筿玛" : "电子锁") : "E-Lock")
#define ESID_Jd_XiaoduWanGui             (g_China ? (g_Big5 ? "瑀窲耫" : "消毒碗柜") : "Sterilizer")
#define ESID_Jd_KongQiJinHua             (g_China ? (g_Big5 ? "瞓て" : "空气净化") : "Air Purifier")
#define ESID_Jd_YinShuiJi                (g_China ? (g_Big5 ? "都诀" : "饮水机") : "Water Dispenser")
#define ESID_Jd_KongTiaoJi               (g_China ? (g_Big5 ? "秸诀" : "空调机") : "A/C 1")
#define ESID_Jd_LenQiJi                  (g_China ? (g_Big5 ? "诀" : "冷气机") : "A/C 2")
#define ESID_Jd_CouYouJi                 (g_China ? (g_Big5 ? "┾猳诀" : "抽油机") : "A/C 3")
#define ESID_Jd_NuanFenJi                (g_China ? (g_Big5 ? "穢诀" : "暖风机") : "A/C 4")
#define ESID_Jd_DianReTan                (g_China ? (g_Big5 ? "筿荐脆" : "电热毯") : "Electric Blanket")
#define ESID_Jd_ReShuiLu                 (g_China ? (g_Big5 ? "荐膌" : "热水炉") : "Water Heater")
#define ESID_Jd_DianFanBao               (g_China ? (g_Big5 ? "筿逗辳" : "电饭煲") : "Rice Cooker")
#define ESID_Jd_DianZhiWaGuo             (g_China ? (g_Big5 ? "筿ニ羚" : "电子瓦锅") : "Electric Stewpot")
#define ESID_Jd_WeiBoLu                  (g_China ? (g_Big5 ? "稬猧膌" : "微波炉") : "Microwave Oven")
#define ESID_Jd_XiYiJi                   (g_China ? (g_Big5 ? "瑍︾诀" : "洗衣机") : "Socket 6")
#define ESID_Jd_DianShiJi                (g_China ? (g_Big5 ? "筿跌诀" : "电视机") : "Socket 1")
#define ESID_Jd_LuXiangJi                (g_China ? (g_Big5 ? "魁紇诀" : "录像机") : "VCR")
#define ESID_Jd_YingXiang                (g_China ? (g_Big5 ? "臫" : "音响") : "Stereo")
#define ESID_Jd_DianNao                  (g_China ? (g_Big5 ? "筿福" : "电脑") : "PC")
#define ESID_Jd_FengShan                 (g_China ? (g_Big5 ? "" : "风扇") : "Fan")
#define ESID_Jd_CD                       (g_China ? (g_Big5 ? "CD" : "CD") : "CD")
#define ESID_Jd_LD                       (g_China ? (g_Big5 ? "LD" : "LD") : "LD")
#define ESID_Jd_VCD                      (g_China ? (g_Big5 ? "VCD" : "VCD") : "VCD")
#define ESID_Jd_DVD                      (g_China ? (g_Big5 ? "DVD" : "DVD") : "DVD")
#define ESID_Jd_ChaZuo1                  (g_China ? (g_Big5 ? "础畒1" : "插座1") : "Socket 1")
#define ESID_Jd_ChaZuo2                  (g_China ? (g_Big5 ? "础畒2" : "插座2") : "Socket 2")
#define ESID_Jd_ChaZuo3                  (g_China ? (g_Big5 ? "础畒3" : "插座3") : "Socket 3")
#define ESID_Jd_ChaZuo4                  (g_China ? (g_Big5 ? "础畒4" : "插座4") : "Socket 4")
#define ESID_Jd_ChaZuo5                  (g_China ? (g_Big5 ? "础畒5" : "插座5") : "Socket 5")
#define ESID_Jd_XiaoDuSheBe              (g_China ? (g_Big5 ? "瑀砞称" : "消毒设备") : "Disinfectant Apparatus")
#define ESID_Jd_SettingMusic             (g_China ? (g_Big5 ? "璉春贾" : "背景音乐") : "Background Music")
#define ESID_Jd_ChuangLian               (g_China ? (g_Big5 ? "怠卯" : "窗帘") : "Curtain")
#define ESID_Jd_JuanLian                 (g_China ? (g_Big5 ? "卯" : "卷帘") : "Shutter")
#define ESID_Jd_DianDongBuLian           (g_China ? (g_Big5 ? "筿笆ガ卯" : "电动布帘") : "Electric Curtain")
#define ESID_Jd_DianDongShaLian          (g_China ? (g_Big5 ? "筿笆卯" : "电动纱帘") : "Electric Gauze")
#define ESID_Jd_SceneNameSet             (g_China ? (g_Big5 ? "嘿砞竚" : "名称设置") : "Scene Name")
#define ESID_Jd_SceneActionSet           (g_China ? (g_Big5 ? "︽砞竚" : "行为设置") : "Scene Action")
 
// 对讲类
#define ESID_MainCall_User				 (g_China ? (g_Big5 ? "㊣め" : "呼叫住户") : "Resident")
#define ESID_MainCall_FengJi			 (g_China ? (g_Big5 ? "㊣だ诀" : "呼叫分机") : "Sub-terminal")
#define ESID_MainCenter_1				 (g_China ? (g_Big5 ? "㊣いみ1" : "呼叫中心1") : "CALL ADMIN1")
#define ESID_MainCenter_2				 (g_China ? (g_Big5 ? "㊣いみ2" : "呼叫中心2") : "CALL ADMIN2")
#define ESID_MainCenter_3				 (g_China ? (g_Big5 ? "㊣いみ3" : "呼叫中心3") : "CALL ADMIN3")
#define ESID_MainMonitor_Door			 (g_China ? (g_Big5 ? "菏跌玡" : "监视门前") : "Doorway")
#define ESID_MainMonitor_Stair			 (g_China ? (g_Big5 ? "菏跌辫" : "监视梯口") : "Sec.Entry")
#define ESID_MainMonitor_Area			 (g_China ? (g_Big5 ? "菏跌跋" : "监视区口") : "Prim.Entry")
#define ESID_MainMonitor_Camera			 (g_China ? (g_Big5 ? "菏跌跋" : "监视社区") : "Monitor Community")
#define ESID_MainMonitor_Room			 (g_China ? (g_Big5 ? "產﹡菏跌" : "家居监视") : "RoomCamera")
#define ESID_DevManager                  (g_China ? (g_Big5 ? "恨瞶诀" : "管理员机") : "Admin")
#define ESID_DevArea                     (g_China ? (g_Big5 ? "跋诀" : "区口机") : "Prim.Entry")
#define ESID_DevStair                    (g_China ? (g_Big5 ? "辫诀" : "梯口机") : "Sec.Entry")
#define ESID_DevRoom                     (g_China ? (g_Big5 ? "ず诀" : "室内机") : "Indoor Terminal")
#define ESID_DevPhone                    (g_China ? (g_Big5 ? "炊硄筿杠" : "普通电话") : "Phone")
#define ESID_DevDoorPhone                (g_China ? (g_Big5 ? "玡诀" : "门前机") : "Camera")
#define ESID_DevDoorNet                  (g_China ? (g_Big5 ? "呼蹈玡诀" : "网络门前机") : "IP Camera")
#define ESID_DevGateWay                  (g_China ? (g_Big5 ? "產畑呼闽" : "家庭网关") : "Gateway")
#define ESID_DevIPCamera                 (g_China ? (g_Big5 ? "IP Camera" : "IP Camera") : "IP Camera")
#define ESID_DevFenJi                    (g_China ? (g_Big5 ? "だ诀" : "分机") : "Extension")
#define ESID_DevMaster                   (g_China ? (g_Big5 ? "诀" : "主机") : "Main Phone")
#define ESID_RecordMiss                  (g_China ? (g_Big5 ? "ゼ钡" : "未接") : "Missed")
#define ESID_RecordIn                    (g_China ? (g_Big5 ? "钡" : "已接") : "Received")
#define ESID_RecordOut                   (g_China ? (g_Big5 ? "挤" : "已拨") : "Dialled")
#define ESID_Right_Full                  (g_China ? (g_Big5 ? "" : "全屏") : "Full Screen")
#define ESID_Inter_HangUP                (g_China ? (g_Big5 ? "本诀" : "挂机") : "Hang Up")
#define ESID_Inter_Answer                (g_China ? (g_Big5 ? "钡钮" : "接听") : "Answer")
#define ESID_Inter_Unlock                (g_China ? (g_Big5 ? "秨玛" : "开锁") : "Open")
#define ESID_Inter_ReDial                (g_China ? (g_Big5 ? "挤" : "重拨") : "Redial")
#define ESID_Right_Call                  (g_China ? (g_Big5 ? "㊣" : "呼叫") : "Call")
#define ESID_Inter_Capture               (g_China ? (g_Big5 ? "╃酚" : "抓拍") : "Capture")
#define ESID_Inter_Volume                (g_China ? (g_Big5 ? "秖" : "音量") : "Volume")
#define ESID_Inter_Next                  (g_China ? (g_Big5 ? "" : "下一个") : "Next")
#define ESID_Inter_Talk                  (g_China ? (g_Big5 ? "硄杠" : "通话") : "Talk")
#define ESID_Inter_GetList               (g_China ? (g_Big5 ? "莉" : "获取列表") : "Obtain")
#define ESID_Inter_Snap                  (g_China ? (g_Big5 ? "╃酚" : "拍照") : "Snap")
#define ESID_Inter_AudioAdd              (g_China ? (g_Big5 ? "秖+" : "音量+") : "Audio Add")
#define ESID_Inter_AudioDel              (g_China ? (g_Big5 ? "秖-" : "音量-") : "Audio Del")
#define ESID_Inter_StopMonitor           (g_China ? (g_Big5 ? "氨ゎ菏跌" : "停止监视") : "Stop")
#define ESID_Input_OK                    (g_China ? (g_Big5 ? "块" : "输入") : "Enter")
#define ESID_Inter_EnterUserNo           (g_China ? (g_Big5 ? "叫块め腹絏" : "请输入住户号码") : "Pls enter a number")
#define ESID_Inter_EnterFJNo             (g_China ? (g_Big5 ? "叫块1だ诀腹絏" : "请输入1位分机号码") : "Pls enter a number")
#define ESID_Inter_WrongNo               (g_China ? (g_Big5 ? "块腹絏岿粇!" : "输入号码错误!") : "Wrong number!")
#define ESID_Inter_NoNotFound            (g_China ? (g_Big5 ? "琩礚腹!" : "查无此号!") : "Number not found!")
#define ESID_Inter_Connecting            (g_China ? (g_Big5 ? "硈钡い..." : "连接中...") : "Connecting...")
#define ESID_Inter_BeCalling             (g_China ? (g_Big5 ? "ㄓ筿い..." : "来电中...") : "Calling from...")
#define ESID_Inter_Calling               (g_China ? (g_Big5 ? "㊣い..." : "呼叫中...") : "Calling...")
#define ESID_Inter_Talking               (g_China ? (g_Big5 ? "硄杠い..." : "通话中...") : "Talking...")
#define ESID_Inter_RecordHinting         (g_China ? (g_Big5 ? "痙ē矗ボい..." : "留言提示中...") : "Record Prompting...")
#define ESID_Inter_Recording             (g_China ? (g_Big5 ? "痙ēい..." : "留言中...") : "Recording...")
#define ESID_Inter_UnlockSuccess         (g_China ? (g_Big5 ? "秨 玛 Θ \!" : "开 锁 成 功!") : "Door Unlocked!")
#define ESID_Inter_UnlockFailed          (g_China ? (g_Big5 ? "秨 玛 ア 毖!" : "开 锁 失 败!") : "Failed!")
#define ESID_Inter_CaptureSuccess        (g_China ? (g_Big5 ? "╃酚Θ\!" : "抓 拍 成 功!") : "OK!")
#define ESID_Inter_CaptureFailed         (g_China ? (g_Big5 ? "╃酚ア毖!" : "抓 拍 失 败!") : "Failed!")
#define ESID_Inter_ManagerNoSet          (g_China ? (g_Big5 ? "いみゼ砞竚!" : "此中心未设置!") : "The admin unit IP not set yet!")
#define ESID_Inter_Search                (g_China ? (g_Big5 ? "穓い,叫祔..." : "搜索中,请稍候...") : "Searching...")
#define ESID_Inter_Monitor               (g_China ? (g_Big5 ? "菏跌い..." : "监视中...") : "Monitoring...")
#define ESID_Inter_busy                  (g_China ? (g_Big5 ? "癸よΓ!" : "对方忙!") : "Busy Line!")
#define ESID_Inter_ConnFailed            (g_China ? (g_Big5 ? "硈钡ア毖!" : "连接失败!") : "Failed!")
#define ESID_Inter_SearchFailed          (g_China ? (g_Big5 ? "ゼ穓砞称!" : "未搜索到设备!") : "Failed!")
#define ESID_Inter_ElevAuthorize         (g_China ? (g_Big5 ? "甭舦" : "授权") : "Authorize")
#define ESID_Inter_ElevAuthOK            (g_China ? (g_Big5 ? "筿辫甭舦Θ\!" : "电梯授权成功!") : "OK!")
#define ESID_Inter_ElevAuthFailed        (g_China ? (g_Big5 ? "筿辫甭舦ア毖!" : "电梯授权失败!") : "Failed!")
#define ESID_Auto_Capture				 (g_China ? (g_Big5 ? "笆╃酚" : "自动抓拍") : "Automatic capture")

// 信息文字
#define ESID_Info_Sms                    (g_China ? (g_Big5 ? "祏獺" : "短信") : "Message")
#define ESID_Info_Centre_Des             (g_China ? (g_Big5 ? "いみ狝竟" : "中心服务器") : "Center Server")
 
// 留言文字
#define ESID_Media_Lyly                  (g_China ? (g_Big5 ? "紇" : "影音") : "Video MSG")
#define ESID_Media_Snap                  (g_China ? (g_Big5 ? "酚" : "照片") : "Picture")
#define ESID_Media_Num                   (g_China ? (g_Big5 ? "羆" : "总共") : "Memory")
#define ESID_Media_Z                     (g_China ? (g_Big5 ? "眎" : "张") : "Pcs")

// 便利文字
#define ESID_Bl_Jrly                     (g_China ? (g_Big5 ? "產痙ē" : "家人留言") : "Recorder")
#define ESID_Bl_ClearScreen              (g_China ? (g_Big5 ? "睲间棵辊" : "清洁屏幕") : "Clean Screen")

// 家电文字
#define ESID_Jd_Mode_Unused              (g_China ? (g_Big5 ? "ゼ币ノ產筿家舱" : "未启用家电模块") : "Automation module inactivated")
#define ESID_Jd_Factory_Set              (g_China ? (g_Big5 ? "產筿紅坝砞竚" : "家电厂商设置") : "Brand Settings")
#define ESID_Jd_Factory                  (g_China ? (g_Big5 ? "產筿紅坝" : "家电厂商") : "Makers")
#define ESID_Jd_Factory_X10              (g_China ? (g_Big5 ? "X10" : "X10") : "X10")
#define ESID_Jd_Factory_Suobo            (g_China ? (g_Big5 ? "痴" : "索博") : "PLC-BUS")
#define ESID_Jd_Factory_ACBUS            (g_China ? (g_Big5 ? "TA-BUS" : "TA-BUS") : "TA-BUS")

// 报警文字
#define ESID_Bj_SOS                      (g_China ? (g_Big5 ? "候厨牡" : "紧急报警") : "SOS")
#define ESID_Bj_Fire                     (g_China ? (g_Big5 ? "╝贝代" : "火灾探测") : "Fire")
#define ESID_Bj_Gas                      (g_China ? (g_Big5 ? "ニ吹贝代" : "瓦斯探测") : "Gas Leak")
#define ESID_Bj_Door                     (g_China ? (g_Big5 ? "獻" : "大门入侵") : "Door")
#define ESID_Bj_Room                     (g_China ? (g_Big5 ? "芔贝代" : "客厅探测") : "Room")
#define ESID_Bj_Big_Window               (g_China ? (g_Big5 ? "怠め1獻" : "窗户1入侵") : "Window1")
#define ESID_Bj_Window                   (g_China ? (g_Big5 ? "怠め2獻" : "窗户2入侵") : "Window2")
#define ESID_Bj_Balcony                  (g_China ? (g_Big5 ? "锭籓獻" : "阳台入侵") : "Balcony")
#define ESID_Bj_Bf_Home                  (g_China ? (g_Big5 ? "產" : "在家") : "Home")
#define ESID_Bj_Bf_Out                   (g_China ? (g_Big5 ? "" : "外出") : "Away")
#define ESID_Bj_Bf_Night                 (g_China ? (g_Big5 ? "丁" : "夜间") : "Night")
#define ESID_Bj_Clean                    (g_China ? (g_Big5 ? "睲埃牡ボ" : "清除警示") : "Reset")
#define ESID_Bj_Baojing_Record           (g_China ? (g_Big5 ? "厨牡癘魁" : "报警记录") : "Alarm Log")
#define ESID_Bj_Caozuo_Rec               (g_China ? (g_Big5 ? "巨癘魁" : "操作记录") : "Operation Log")
#define ESID_Bj_Alarm_Rec_Clear_F        (g_China ? (g_Big5 ? "癘魁睲贰ア毖!" : "记录清除失败!") : "Failed!")
#define ESID_Bj_Fangqu1					 (g_China ? (g_Big5 ? "ň跋1" : "防区1") : "Zone1")
#define ESID_Bj_Fangqu2					 (g_China ? (g_Big5 ? "ň跋2" : "防区2") : "Zone2")
#define ESID_Bj_Fangqu3					 (g_China ? (g_Big5 ? "ň跋3" : "防区3") : "Zone3")
#define ESID_Bj_Fangqu4					 (g_China ? (g_Big5 ? "ň跋4" : "防区4") : "Zone4")
#define ESID_Bj_Fangqu5					 (g_China ? (g_Big5 ? "ň跋5" : "防区5") : "Zone5")
#define ESID_Bj_Fangqu6					 (g_China ? (g_Big5 ? "ň跋6" : "防区6") : "Zone6")
#define ESID_Bj_Fangqu7					 (g_China ? (g_Big5 ? "ň跋7" : "防区7") : "Zone7")
#define ESID_Bj_Fangqu8					 (g_China ? (g_Big5 ? "ň跋8" : "防区8") : "Zone8")
#define ESID_Bj_Fangqu					 (g_China ? (g_Big5 ? "ň跋" : "防区") : "Zone")
#define ESID_Bj_Set_Err                  (g_China ? (g_Big5 ? "叫產家Α秈︽砞竚!" : "请在在家模式下进行设置!") : "Please disarm the system first!")
#define ESID_Bj_Fangqu_Settings          (g_China ? (g_Big5 ? "ň跋砞竚" : "防区设置") : "Particular Parameters")
#define ESID_Bj_Security_Settings        (g_China ? (g_Big5 ? "ň砞竚" : "安防设置") : "Security Settings")
#define ESID_Bj_Security_Parameter       (g_China ? (g_Big5 ? "ň把计" : "安防参数") : "General Parameters") 
#define ESID_Bj_Tantou_Type              (g_China ? (g_Big5 ? "贝代竟摸" : "探头类型") : "Protection")
#define ESID_Bj_24Hour_Fangqu            (g_China ? (g_Big5 ? "24ň跋" : "24小时防区") : "24H Zone")
#define ESID_Bj_Normally_Open            (g_China ? (g_Big5 ? "贝代竟盽秨" : "探头常开") : "N/O")
#define ESID_Bj_Normally_Close           (g_China ? (g_Big5 ? "贝代竟盽超" : "探头常闭") : "N/C")
#define ESID_Bj_Alarm_Delay              (g_China ? (g_Big5 ? "厨牡┑筐" : "报警延迟") : "Entry Delay")
#define ESID_Bj_None                     (g_China ? (g_Big5 ? "礚" : "无") : "0 Sec")
#define ESID_Bj_Picture                  (g_China ? (g_Big5 ? "礶" : "画面") : "Picture")
#define ESID_Bj_Voice                    (g_China ? (g_Big5 ? "羘" : "声音") : "Sound")
#define ESID_Bj_Enable                   (g_China ? (g_Big5 ? "币ノ" : "启用") : "Activate")
#define ESID_Disable                     (g_China ? (g_Big5 ? "窽ノ" : "禁用") : "Deactivate")
#define ESID_Bj_30Seconds			     (g_China ? (g_Big5 ? "30" : "30秒") : "30Sec")
#define ESID_Bj_1Minute				     (g_China ? (g_Big5 ? "1だ" : "1分") : "1Min")
#define ESID_Bj_2Minutes				 (g_China ? (g_Big5 ? "2だ" : "2分") : "2Min")
#define ESID_Bj_3Minutes				 (g_China ? (g_Big5 ? "3だ" : "3分") : "3Min")
#define ESID_Bj_4Minutes				 (g_China ? (g_Big5 ? "4だ" : "4分") : "4Min")
#define ESID_Bj_5Minutes				 (g_China ? (g_Big5 ? "5だ" : "5分") : "5Min")
#define ESID_Bj_10Minutes			     (g_China ? (g_Big5 ? "10だ" : "10分") : "10Min")
#define ESID_Bj_15Minutes			     (g_China ? (g_Big5 ? "15だ" : "15分") : "15Min")    
#define ESID_Bj_Alarm_Time               (g_China ? (g_Big5 ? "牡筧" : "警铃时长") : "Alarm Duration")
#define ESID_Bj_Early_Warning_Time       (g_China ? (g_Big5 ? "箇牡丁" : "预警时间") : "Exit Delay")
#define ESID_Bj_Early_Warning_Voice      (g_China ? (g_Big5 ? "箇牡羘" : "预警声") : "Exit Alert") 
#define ESID_Bj_Remote_sz                (g_China ? (g_Big5 ? "环祘砞竚" : "远程设置") : "Remote Settings")
#define ESID_Bj_Remote_bf                (g_China ? (g_Big5 ? "环祘ň" : "远程布防") : "Remote Arm(AWAY)")
#define ESID_Bj_Remote_cf                (g_China ? (g_Big5 ? "环祘篗ň" : "远程撤防") : "Remote Disarm(HOME)")
#define ESID_Bj_Remote_jf                (g_China ? (g_Big5 ? "环祘Ыň" : "远程局防") : "Remote Arm(NIGHT)")
#define ESID_Bj_Already_Bufang           (g_China ? (g_Big5 ? "莱ň跋秈︽虫縒ň" : "按相应的防区可进行单独布防") : "Tap a zone to arm it")
#define ESID_Bj_Report_Type_Chufa        (g_China ? (g_Big5 ? "牟祇厨牡" : "触发报警") : "Triggerred Alarm")
#define ESID_Bj_Report_Type_Qiuzhu       (g_China ? (g_Big5 ? "―厨牡" : "求助报警") : "HELP")
#define ESID_Bj_Alarm                    (g_China ? (g_Big5 ? "厨牡" : "报警") : "Alarm")
#define ESID_Bj_Report_Type_Jinru        (g_China ? (g_Big5 ? "秈厨牡" : "进入报警") : "Alarm")
#define ESID_Bj_Fangquchufa              (g_China ? (g_Big5 ? "ň跋牟祇" : "防区触发") : "Zone Triggered!") 
#define ESID_Bj_Bengjikongzhi            (g_China ? (g_Big5 ? "セ诀北" : "本机控制") : "Local Control") 
#define ESID_Bj_Fengjikongzhi            (g_China ? (g_Big5 ? "だ诀北" : "分机控制") : "Sub-phone Control") 
#define ESID_Bj_Query_Del_Rec_One        (g_China ? (g_Big5 ? "絋﹚埃赣癘魁?" : "确定删除该记录?") : "Sure to delete it?")  
#define ESID_Bj_Query_Del_Rec_All        (g_China ? (g_Big5 ? "絋﹚睲┮Τ癘魁?" : "确定清空所有记录?") : "Sure to delete all?") 
#define ESID_Bj_Chufa                    (g_China ? (g_Big5 ? "牟祇" : "触发") : "Triggerred")
#define ESID_Bj_Cannot_Bufang            (g_China ? (g_Big5 ? "ぃň" : "不能布防") : "Cannot be armed!")
#define ESID_InputBox_Psw_Err            (g_China ? (g_Big5 ? "盞絏岿粇,叫穝块" : "密码错误,请重新输入") : "Wrong Password!")
#define ESID_Isolation                   (g_China ? (g_Big5 ? "筳瞒" : "隔离") : "Bypass")
#define ESID_AfJoint_Tk_Chefang          (g_China ? (g_Big5 ? "辫篗ň" : "梯口刷卡撤防") : "Disarm By Card(sec.entry)") 
#define ESID_AfJoint_Mk_Chefang          (g_China ? (g_Big5 ? "玡篗ň" : "门前刷卡撤防") : "Disarm By Card(doorway)") 
#define ESID_AfJoint_DoorRing            (g_China ? (g_Big5 ? "材ň跋讽玡筧" : "第八防区当门前铃") : "Enable zone 8 to work as doorbell")
#define ESID_AfJoint_ScenBf              (g_China ? (g_Big5 ? "ň羛笆薄春家Α" : "布防联动情景模式") : "Activate scene mode X under the AWAY mode")
#define ESID_AfJoint_ScenCf              (g_China ? (g_Big5 ? "篗ň羛笆薄春家Α" : "撤防联动情景模式") : "Activate scene mode X under the HOME mode")
#define ESID_Af_Tel_Num1                 (g_China ? (g_Big5 ? "厨牡腹絏1:" : "报警号码1:") : "Phone No.1")
#define ESID_Af_Tel_Num2                 (g_China ? (g_Big5 ? "厨牡腹絏2:" : "报警号码2:") : "Phone No.2")
#define ESID_Af_Msg_Num1                 (g_China ? (g_Big5 ? "祏獺腹絏1:" : "短信号码1:") : "MSG No.1")
#define ESID_Af_Msg_Num2                 (g_China ? (g_Big5 ? "祏獺腹絏2:" : "短信号码2:") : "MSG No.2")
#define ESID_Af_Mode_Unused              (g_China ? (g_Big5 ? "ゼ币ノ厨牡家舱" : "未启用报警模块") : "Alarm module inactivated!")
#define ESID_Af_Alarm_Cannot_Bf          (g_China ? (g_Big5 ? "タ厨牡ぃň" : "正在报警不能布防") : "Alarming now. Cannot arm!")
#define ESID_Af_Touch_Cannot_Bf          (g_China ? (g_Big5 ? "ň跋牟祇ぃň" : "防区触发不能布防") : "Zone triggerred.Cannot arm!")
#define ESID_Af_Alarm_Cannot_Qc          (g_China ? (g_Big5 ? "タ厨牡ぃ巨" : "正在报警不能操作") : "Zone triggerred.Cannot operate!")

// 预约文字
#define ESID_Jd_Yuyue                    (g_China ? (g_Big5 ? "箇" : "预约") : "Timer")
#define ESID_Jd_Yuyue_Manager            (g_China ? (g_Big5 ? "箇恨瞶" : "预约管理") : "Timing Control")
#define ESID_Jd_Yuyue_All                (g_China ? (g_Big5 ? "场" : "全部") : "All")
#define ESID_Jd_Yuyue_DelOne             (g_China ? (g_Big5 ? "絋﹚埃赣箇?" : "确定删除该预约?") : "Sure to delete it?")
#define ESID_Jd_Yuyue_Add_Err            (g_China ? (g_Big5 ? "箇ぃ禬筁20兵!" : "预约不能超过20条!") : "Memory Full!")
#define ESID_Jd_Yuyue_EveryDay           (g_China ? (g_Big5 ? "–ぱ" : "每天") : "Daily")
#define ESID_Jd_Yuyue_EveryWeek          (g_China ? (g_Big5 ? "–㏄" : "每周") : "Weekly")
#define ESID_Jd_Yuyue_EveryMonth         (g_China ? (g_Big5 ? "–る" : "每月") : "Monthly")
#define ESID_Jd_Yuyue_Day                (g_China ? (g_Big5 ? "ら" : "日") : "Day")
#define ESID_Jd_Yuyue_Action_Open        (g_China ? (g_Big5 ? "秨" : "开") : "ON")
#define ESID_Jd_Yuyue_Action_Close       (g_China ? (g_Big5 ? "闽" : "关") : "OFF")
#define ESID_Jd_Yuyue_Type               (g_China ? (g_Big5 ? "摸" : "类型") : "Type")
#define ESID_Jd_Yuyue_Name               (g_China ? (g_Big5 ? "嘿" : "名称") : "Name")
#define ESID_Jd_Yuyue_Status             (g_China ? (g_Big5 ? "篈" : "状态") : "Status")
#define ESID_Jd_Yuyue_Repeat             (g_China ? (g_Big5 ? "狡よΑ" : "重复方式") : "Repeat")
#define ESID_Jd_Yuyue_StartTime          (g_China ? (g_Big5 ? "秨﹍丁" : "开始时间") : "Start Time")

// 社区监视文字
#define ESID_Rtsp_IPC                    (g_China ? (g_Big5 ? "呼蹈IPC" : "网络IPC") : "Net IPC")
#define ESID_Rtsp_Name                   (g_China ? (g_Big5 ? "砞称嘿" : "设备名称") : "Device Name")
#define ESID_Rtsp_Port                   (g_China ? (g_Big5 ? "PORT" : "PORT") : "PORT")
#define ESID_Rtsp_Channel                (g_China ? (g_Big5 ? "跌繵硄笵" : "视频通道") : "Video Channel")
#define ESID_Rtsp_User                   (g_China ? (g_Big5 ? "ノめ" : "用户名") : "User")
#define ESID_Rtsp_Password               (g_China ? (g_Big5 ? "盞絏" : "密码") : "Password")
#define ESID_Rtsp_IP                 	 (g_China ? (g_Big5 ? "IP" : "IP地址") : "IP Address")
#define ESID_Rtsp_Factory                (g_China ? (g_Big5 ? "紅坝嘿" : "厂商名称") : "Manufacturer")

// 家居监视文字
#define ESID_Rtsp_Indoor_Camera          (g_China ? (g_Big5 ? "ず尼紇诀" : "室内摄像头") : "Indoor Camera")
#define ESID_Rtsp_Enable_Indoor_Camera   (g_China ? (g_Big5 ? "币ノず尼紇诀" : "启用室内摄像头") : "Enable Indoor Camera")
#define ESID_Rtsp_PTZ                	 (g_China ? (g_Big5 ? "币ノPTZ北" : "启用PTZ控制") : "Enable PTZ Control")
#define ESID_Rtsp_Direc				 	 (g_China ? (g_Big5 ? "よ" : "方向") : "Direct")
#define ESID_Rtsp_Scaling				 (g_China ? (g_Big5 ? "罽" : "缩放") : "Scaling")
#define ESID_Rtsp_Point				     (g_China ? (g_Big5 ? "箇竚翴" : "预置点") : "Point")
#define ESID_Rtsp_Point1				 (g_China ? (g_Big5 ? "箇竚翴1" : "预置点1") : "Point 1")
#define ESID_Rtsp_Point2				 (g_China ? (g_Big5 ? "箇竚翴2" : "预置点2") : "Point 2")
#define ESID_Rtsp_Point3				 (g_China ? (g_Big5 ? "箇竚翴3" : "预置点3") : "Point 3")
#define ESID_Rtsp_Point4				 (g_China ? (g_Big5 ? "箇竚翴4" : "预置点4") : "Point 4")

//蓝牙门前机文字
#ifdef _ADD_BLUETOOTH_DOOR_
#define ESID_Ble_Door_Unused1       	 (g_China ? (g_Big5 ? "ゼ币ノ屡玡诀!" : "未启用蓝牙门前机一!") : "Bluetooth Camera 1 Inactivated!")
#define ESID_Ble_Door_Unused2       	 (g_China ? (g_Big5 ? "ゼ币ノ屡玡诀!" : "未启用蓝牙门前机二!") : "Bluetooth Camera 2 Inactivated!")
#define ESID_Ble_Door_First_Record 		 (g_China ? (g_Big5 ? "玡诀癘魁" : "门前机一记录") : "Camera 1 Register")
#define ESID_Ble_Door_Second_Record 	 (g_China ? (g_Big5 ? "玡诀癘魁" : "门前机二记录") : "Camera 2 Register")
#define ESID_Ble_Door_Operate_Record 	 (g_China ? (g_Big5 ? "巨癘魁" : "操作记录") : "Operation Log")
#define ESID_Ble_Door_Lock_Record  		 (g_China ? (g_Big5 ? "秨玛癘魁" : "开锁记录") : "Unlock Log")
#define ESID_Ble_Door_24H_Record   		 (g_China ? (g_Big5 ? "24癘魁" : "24小时记录") : "24-hour Log")
#define ESID_Ble_Door_History_Record 	 (g_China ? (g_Big5 ? "菌癘魁" : "历史记录") : "History Log")
#define ESID_Ble_Door_Snap_Record	 	 (g_China ? (g_Big5 ? "╃酚癘魁" : "拍照记录") : "Snapshot Log")
#define ESID_Ble_Door_First        		 (g_China ? (g_Big5 ? "屡玡诀" : "蓝牙门前机一") : "Bluetooth Camera 1")
#define ESID_Ble_Door_Second       		 (g_China ? (g_Big5 ? "屡玡诀" : "蓝牙门前机二") : "Bluetooth Camera 2")
#define ESID_Set_Unused              	 (g_China ? (g_Big5 ? "ゼ币ノ" : "未启用") : "Not Enabled")
#define ESID_Ble_Door_Obtain       		 (g_China ? (g_Big5 ? "莉い,叫单..." : "获取中,请稍候...") : "Obtain...")
#define ESID_Ble_Door_Obtain_Err   		 (g_China ? (g_Big5 ? "莉癘魁ア毖" : "获取记录失败") : "Obtain Register Error")
#define ESID_Ble_Door_Lock_Key  	 	 (g_China ? (g_Big5 ? "诀瘪秨玛" : "机械开锁") : "Mechanical unlocking")
#define ESID_Ble_Door_Lock_Pwd	 	 	 (g_China ? (g_Big5 ? "盞絏秨玛" : "密码开锁") : "Password access")
#define ESID_Ble_Door_Lock_Card    		 (g_China ? (g_Big5 ? "秨玛" : "刷卡开锁") : "Card access")
#define ESID_Ble_Door_Lock_Finger  		 (g_China ? (g_Big5 ? "秨玛" : "指纹开锁") : "Fingerprint recognition")
#define ESID_Ble_Door_Lock_Face    		 (g_China ? (g_Big5 ? "羪醚秨玛" : "人脸识别开锁") : "Face recognition")
#define ESID_Ble_Door_Lock_Guest   		 (g_China ? (g_Big5 ? "砐秨玛" : "访客开锁") : "Unlock by pressing open key")
#define ESID_Ble_Door_Lock_Monitor 		 (g_China ? (g_Big5 ? "菏跌秨玛" : "监视开锁") : "Unlock when monitoring")
#define ESID_Ble_Door_Lock_Photo   		 (g_China ? (g_Big5 ? "も诀秨玛" : "手机开锁") : "Unlock via mobile phone")
#define ESID_Ble_Door_Lock_Linshi  		 (g_China ? (g_Big5 ? "羬甭舦秨玛" : "临时授权开锁") : "Temporary authorized access")
#define ESID_Ble_Door_Lock_Custom  		 (g_China ? (g_Big5 ? "﹚竡" : "自定义") : "Custom")
#define ESID_Ble_Door_Operate_Del  		 (g_China ? (g_Big5 ? "埃ノめ" : "删除用户") : "Delete user account")
#define ESID_Ble_Door_Operate_Open 		 (g_China ? (g_Big5 ? "砞竚盽秨" : "设置门常开") : "N/O on")
#define ESID_Ble_Door_Operate_Unopen 	 (g_China ? (g_Big5 ? "盽秨" : "取消门常开") : "N/O off")
#define ESID_Ble_Door_Operate_Ring 		 (g_China ? (g_Big5 ? "筧" : "振铃") : "Ringtone")
#define ESID_Ble_Door_Dev_Host 		 	 (g_China ? (g_Big5 ? "セ诀" : "本机") : "Device")
#define ESID_Ble_Door_Dev_Door 	 		 (g_China ? (g_Big5 ? "玡诀" : "门前机") : "Camera")
#define ESID_Ble_Door_Dev_Photo 		 (g_China ? (g_Big5 ? "も诀" : "手机") : "Mobile phone")
#define ESID_Ble_Door_ManInduction 	     (g_China ? (g_Big5 ? "砰稰莱╃酚" : "人体感应拍照") : "Snap by human induction")
#define ESID_Ble_Door_ThreeErr		 	 (g_China ? (g_Big5 ? "きΩ巨岿粇╃酚" : "五次操作错误拍照") : "Five Times Operate Error Snap")
#define ESID_Ble_Door_TamperAlarm 	 	 (g_China ? (g_Big5 ? "ň╊厨牡╃酚" : "防拆报警拍照") : "Anti demolition alarm snap")
#define ESID_Ble_Door_Visitor 	 	 	 (g_China ? (g_Big5 ? "砐㊣╃酚" : "访客呼叫拍照") : "Visitors call snap")
#define ESID_Ble_Door1_Snap_Record 		 (g_China ? (g_Big5 ? "玡诀╃酚癘魁" : "门前机一拍照记录") : "Camera 1 Snapshot Record")
#define ESID_Ble_Door2_Snap_Record 		 (g_China ? (g_Big5 ? "玡诀╃酚癘魁" : "门前机二拍照记录") : "Camera 2 Snapshot Record")
#define ESID_Ble_Door1_Lock24h_Record 	 (g_China ? (g_Big5 ? "玡诀24秨玛癘魁" : "门前机一24小时开锁记录") : "Camera 1 24H Unlock Record")
#define ESID_Ble_Door2_Lock24h_Record 	 (g_China ? (g_Big5 ? "玡诀24秨玛癘魁" : "门前机二24小时开锁记录") : "Camera 2 24H Unlock Record")
#define ESID_Ble_Door1_LockHis_Record 	 (g_China ? (g_Big5 ? "玡诀菌秨玛癘魁" : "门前机一历史开锁记录") : "Camera 1 Unlock Record")
#define ESID_Ble_Door2_LockHis_Record 	 (g_China ? (g_Big5 ? "玡诀菌秨玛癘魁" : "门前机二历史开锁记录") : "Camera 2 Unlock Record")
#define ESID_Ble_Door1_Operate24h_Record (g_China ? (g_Big5 ? "玡诀24巨癘魁" : "门前机一24小时操作记录") : "Camera 1 24H Operate Record")
#define ESID_Ble_Door2_Operate24h_Record (g_China ? (g_Big5 ? "玡诀24巨癘魁" : "门前机二24小时操作记录") : "Camera 2 24H Operate Record")
#define ESID_Ble_Door1_OperateHis_Record (g_China ? (g_Big5 ? "玡诀菌巨癘魁" : "门前机一历史操作记录") : "Camera 1 Operate Record")
#define ESID_Ble_Door2_OperateHis_Record (g_China ? (g_Big5 ? "玡诀菌巨癘魁" : "门前机二历史操作记录") : "Camera 1 Operate Record")
#define ESID_Ble_Door_Room_List 		 (g_China ? (g_Big5 ? "ず诀竕﹚" : "室内机绑定列表") : "Indoor Unit information list")
#define ESID_Ble_Door_DNS_Address 		 (g_China ? (g_Big5 ? "DNS" : "DNS地址") : "DNS Address")
#endif


typedef enum
{
	// 主界面字符ID
	SID_NULL = 1000,
	SID_MainAlarm,
	SID_MainCall,
	SID_MainInfo,
	SID_MainCenter,
	SID_MainMonitor,	
	SID_MainRecord,
	SID_MainLyLy,
	SID_MainBianli,	
    SID_MainBianLi_Dianti,
	SID_MainSet,
    SID_MainEcontrol,
    SID_MainMedia,

    // 公共文字字符ID
    SID_Unregistered,
    SID_Registered,
    SID_Touch_Calibration,          
    SID_Touch_Disp_Chk_Hit,          
    SID_clearscreenhit,            
    SID_Right_Add,                  
    SID_Right_Edit,                  
    SID_Right_Del,                  
    SID_Right_Clear,                 
    SID_Right_Save,                
    SID_Right_View,                
    SID_Right_Sure,                 
    SID_Right_desk,                 
    SID_Right_Rec,
    SID_Right_Listen,                
	SID_Right_Play,               
	SID_Right_Stop,                
	SID_Right_Pause,
	SID_Right_Updata,                
	SID_Right_Select,
	SID_Right_Input,
	SID_Week_Sun,                   
	SID_Week_Mon,                    
	SID_Week_Tue,                   
	SID_Week_Wed,                    
	SID_Week_Thu,
	SID_Week_Fri,          
	SID_Week_Sat,                
	SID_Time_Sec,                   
	SID_Time_Hour,                  
	SID_Time_Min, 
	SID_Time_Never,
	SID_Set_Time,                  
	SID_Set_Mode,                 
	SID_Set_Used,                   
	SID_Save_OK,                
	SID_Save_ERR, 
	SID_Msg_Yes,                     
	SID_Msg_No,                    
	SID_Msg_Format,            
	SID_Oper_OK,                  
	SID_Oper_ERR,                 
	SID_Msg_Calibration,             
	SID_Msg_OldPass_Err,             
	SID_Msg_SamePass_Err,            
	SID_Msg_InputBox_Psw_Err,       
	SID_Msg_Jrly_rec_tmp,
	SID_Msg_Jrly_rec_full,           
	SID_Msg_Connect_Outtime,         
	SID_Msg_Alarm_Hit,              
	SID_Msg_Info_Hit,               
	SID_Msg_CallIn_Hit,            
	SID_Msg_FactoryMode,             
	SID_Msg_Cell_Err,               
	SID_Msg_Stair_Err,             
	SID_Msg_Room_Err,                
	SID_Msg_RuleAll_Err,           
	SID_Msg_Len_Err,              
	SID_Msg_RoomNo_Err,              
	SID_Msg_Recover_OK,             
	SID_Msg_Exit,                   
	SID_Msg_Dianti_Hit1,            
	SID_Msg_Dianti_Hit2,            
	SID_Msg_Reset,                   
	SID_Msg_JD_Is_Updata,            
	SID_Msg_JD_Is_Updata1,           
	SID_Msg_JD_Updataing,
	SID_Msg_JD_Unbind,               
	SID_Msg_Create_BindCode,
	SID_IP_Unbind,
	SID_IP_Bind,                    
	SID_Msg_Jd_State_Updata,         
	SID_IP_Online,                   
	SID_IP_Unline,                  
	SID_IP_Con_Addr,                 
	SID_Jd_Deng1,                    
	SID_Jd_Deng2,                    
	SID_Jd_Deng3,                    
	SID_Jd_Deng4,                    
	SID_Jd_Deng5,                   
	SID_Jd_Deng6,                 
	SID_Jd_Deng7,                  
	SID_Jd_Deng8,                    
	SID_Jd_Deng9,                   
	SID_Jd_Deng10,                   
	SID_Jd_TiaoGuangDeng1,           
	SID_Jd_TiaoGuangDeng2,
	SID_Jd_TiaoGuangDeng3,           
	SID_Jd_TiaoGuangDeng4,           
	SID_Jd_TiaoGuangDeng5,          
	SID_Jd_TiaoGuangDeng6,           
	SID_Jd_TiaoGuangDeng7,          
	SID_Jd_TiaoGuangDeng8,          
	SID_Jd_TiaoGuangDeng9,           
	SID_Jd_TiaoGuangDeng10,          
	SID_Jd_BaiChiDeng1,              
	SID_Jd_BaiChiDeng2,             
	SID_Jd_BaiChiDeng3,             
	SID_Jd_BaiChiDeng4,             
	SID_Jd_BaiChiDeng5,             
	SID_Jd_BaiChiDeng6,              
	SID_Jd_BaiChiDeng7,           
	SID_Jd_BaiChiDeng8,            
	SID_Jd_BaiChiDeng9,              
	SID_Jd_BaiChiDeng10,
	SID_Jd_GuangGuan1,               
	SID_Jd_GuangGuan2,               
	SID_Jd_GuangGuan3,               
	SID_Jd_GuangGuan4,               
	SID_Jd_GuangGuan5,               
	SID_Jd_GuangGuan6,               
	SID_Jd_GuangGuan7,              
	SID_Jd_GuangGuan8,               
	SID_Jd_GuangGuan9,               
	SID_Jd_GuangGuan10,            
	SID_Jd_DiaoDeng1,                
	SID_Jd_DiaoDeng2,                
	SID_Jd_DiaoDeng3,                
	SID_Jd_DiaoDeng4,                
	SID_Jd_DiaoDeng5,               
	SID_Jd_DiaoDeng6,                
	SID_Jd_DiaoDeng7,                
	SID_Jd_DiaoDeng8,                
	SID_Jd_DiaoDeng9,                
	SID_Jd_DiaoDeng10,
	SID_Jd_HuaDeng1,               
	SID_Jd_HuaDeng2,                
	SID_Jd_HuaDeng3,                 
	SID_Jd_HuaDeng4,                
	SID_Jd_HuaDeng5,                 
	SID_Jd_HuaDeng6,                
	SID_Jd_HuaDeng7,               
	SID_Jd_HuaDeng8,                 
	SID_Jd_HuaDeng9,                 
	SID_Jd_HuaDeng10,                
	SID_Jd_BiDeng1,                  
	SID_Jd_BiDeng2,                  
	SID_Jd_BiDeng3,                  
	SID_Jd_BiDeng4,                  
	SID_Jd_BiDeng5,                 
	SID_Jd_BiDeng6,                  
	SID_Jd_BiDeng7,                 
	SID_Jd_BiDeng8,                  
	SID_Jd_BiDeng9,                  
	SID_Jd_BiDeng10,
	SID_Jd_SheDeng1,
	SID_Jd_SheDeng2,
	SID_Jd_SheDeng3,
	SID_Jd_SheDeng4,
	SID_Jd_SheDeng5,
	SID_Jd_SheDeng6,
	SID_Jd_SheDeng7,
	SID_Jd_SheDeng8,
	SID_Jd_SheDeng9,
	SID_Jd_SheDeng10,
	SID_Jd_TaiDeng1,
	SID_Jd_TaiDeng2,
	SID_Jd_TaiDeng3,
	SID_Jd_TaiDeng4,
	SID_Jd_TaiDeng5,
	SID_Jd_TaiDeng6,
	SID_Jd_TaiDeng7,
	SID_Jd_TaiDeng8,
	SID_Jd_TaiDeng9,
	SID_Jd_TaiDeng10,
	SID_Jd_LouDiDeng1,             
	SID_Jd_LouDiDeng2,               
	SID_Jd_LouDiDeng3,          
	SID_Jd_LouDiDeng4,              
	SID_Jd_LouDiDeng5,               
	SID_Jd_LouDiDeng6,               
	SID_Jd_LouDiDeng7,               
	SID_Jd_LouDiDeng8,             
	SID_Jd_LouDiDeng9,               
	SID_Jd_LouDiDeng10,            
	SID_Jd_ZhaoMingDeng1,           
	SID_Jd_ZhaoMingDeng2,           
	SID_Jd_ZhaoMingDeng3,         
	SID_Jd_ZhaoMingDeng4,           
	SID_Jd_ZhaoMingDeng5,            
	SID_Jd_ZhaoMingDeng6,           
	SID_Jd_ZhaoMingDeng7,            
	SID_Jd_ZhaoMingDeng8,           
	SID_Jd_ZhaoMingDeng9,
	SID_Jd_ZhaoMingDeng10,
	SID_Jd_ShiYingDeng1,             
	SID_Jd_ShiYingDeng2,             
	SID_Jd_ShiYingDeng3,            
	SID_Jd_ShiYingDeng4,             
	SID_Jd_ShiYingDeng5,       
	SID_Jd_ShiYingDeng6,             
	SID_Jd_ShiYingDeng7,           
	SID_Jd_ShiYingDeng8,             
	SID_Jd_ShiYingDeng9,            
	SID_Jd_ShiYingDeng10,           
	SID_Jd_YeDeng1,                  
	SID_Jd_YeDeng2,                 
	SID_Jd_YeDeng3,                  
	SID_Jd_YeDeng4,                  
	SID_Jd_YeDeng5,                 
	SID_Jd_YeDeng6,               
	SID_Jd_YeDeng7,                  
	SID_Jd_YeDeng8,                  
	SID_Jd_YeDeng9,                  
	SID_Jd_YeDeng10,
	SID_Jd_ZhuanshiDEng1,            
	SID_Jd_ZhuanshiDEng2,            
	SID_Jd_ZhuanshiDEng3,            
	SID_Jd_ZhuanshiDEng4,            
	SID_Jd_ZhuanshiDEng5,           
	SID_Jd_ZhuanshiDEng6,            
	SID_Jd_ZhuanshiDEng7,            
	SID_Jd_ZhuanshiDEng8,           
	SID_Jd_ZhuanshiDEng9,     
	SID_Jd_ZhuanshiDEng10,

	// 设置类字符ID
	SID_Set_Menu_System,             
	SID_Set_Menu_Project,           
	SID_Set_Menu_Alarm,              
	SID_Set_Menu_User,               
	SID_Set_System_language,         
	SID_Set_System_Time,           
	SID_Set_System_Desk,             
	SID_Set_System_Screen,          
	//SID_Set_System_Light,            
	SID_Set_System_Storage,          
	SID_Set_System_Info,             
	SID_Set_Prj_Pass,                
	SID_Set_Prj_Alarm,               
	SID_Set_Prj_Net,            
	SID_Set_Prj_ExtMode,            
	SID_Set_Prj_DevNo,            
	SID_Set_Prj_NetDoor,             
	SID_Set_Prj_Default,          
	SID_Set_Prj_JiaDian,            
	//SID_Set_Prj_Mic,
	//SID_Set_Prj_Mic_Db,             
	SID_Set_Prj_Set_Rtsp,            
	SID_Set_Prj_Set_IPModule,        
	SID_Set_Alarm_UserPwd,           
	SID_Set_Alarm_DoorPwd,          
	SID_Set_Alarm_AfLink,            
	SID_Set_Alarm_AfGeLi,           
	SID_Set_Alarm_AfJuFang,        
	SID_Set_UAlarm_Remote,           
	SID_Set_Alarm_MsgNum,           
	//SID_Set_Alarm_TelNum,           
	SID_Set_User_Sound,              
	SID_Set_User_Volume,            
	SID_Set_User_NoFace,             
	SID_Set_User_LyLy,               
	SID_Set_User_PT,                 
	SID_Set_Unreg,                   
	SID_Set_Reg_Code,                
	SID_Set_Reging,                  
	SID_Set_Prj_Net_HostIP,
	SID_Set_Prj_Net_HostNetMask,        
	SID_Set_Prj_Net_HostGateWay,     
	SID_Set_Prj_Net_CenterSever,     
	SID_Set_Prj_Net_ManageIP1,  
	SID_Set_Prj_Net_ManageIP2,       
	SID_Set_Prj_Net_ManageIP3,  
	SID_Set_Prj_Net_AurineSever, 
	SID_Set_Prj_Net_AurineElevator,   
	SID_Set_Prj_Net_Rtsp,  	
	SID_Set_Prj_Net_Stair,  
	SID_Set_Prj_Net_IPModule_IP,        
	SID_Set_Prj_Net_IPModule_Code,        
	SID_Set_Dev_Rule,                
	SID_Set_Dev_Stair_len,           
	SID_Set_Dev_Room_len,           
	SID_Set_Dev_Cell_len,           
	SID_Set_Dev_UseCell,             
	SID_Set_Dev_BIT,                 
	SID_Set_Ext_Alarm,              
	SID_Set_Ext_JiaDian,           
	SID_Set_Ext_Stair,
	SID_Set_Language,
	#ifdef _TCPTO485_ELEVATOR_MODE_
	SID_Set_TCP485Elevator,
	#endif
    
    // 网络门前机文字字符ID
    SID_Net_Door_First_R,
	SID_Net_Door_Second_R,
    SID_Net_Door_First_A,
	SID_Net_Door_Second_A,
	SID_Net_Door,
	SID_Net_Door_First,          
	SID_Net_Door_Second,           
	SID_Net_Door_IP,                 
	SID_Net_Door_Sub,                
	SID_Net_Door_GateWay, 
	SID_Net_Door_MAC,  
	SID_Net_Door_GetMAC,                          
	SID_Net_Door_NetParam,          
	SID_Net_Door_LockType,           
	SID_Net_Door_CardMan,            
	SID_Net_Door_SysInfo,          
	SID_Net_Door_Other_Set,        
	SID_Net_Door_Open,              
	SID_Net_Door_Close,            
	SID_Net_Door_LTime_Mh,          
	SID_Net_Door_LockTime,
	SID_Net_Door_Pic_Up,          
	SID_Net_Door_Remote_Moniter,    
	SID_Net_Door_Add_Card,           
	SID_Net_Door_Del_Card,           
	SID_Net_Door_Unused_First,      
	SID_Net_Door_Unused_Second,    
	SID_Net_Door_Pls_Add_Card,       
	SID_Net_Door_Sure_Del_Card,      
	SID_Net_Door_Video_Fmt,          
	SID_Set_Info_HW,                 
	SID_Set_Info_SF,                
	SID_Set_Info_SDK,              
	SID_Set_Info_HostMAC,           
	SID_Set_Info_Room,               
	SID_Set_Timer_Wanl,              
	SID_Storage_Format,              
	SID_Storage_Pic,                 
	SID_Set_Pass_Server,             
	SID_Set_Pass_DoorUser,          
	SID_Set_Pass_DoorServer, 
	SID_Set_Pass_Old,                
	SID_Set_Pass_New,               
	SID_Set_Pass_Query,              
	SID_Set_volume_ring,            
	SID_Set_volume_talk,             
	SID_Set_volume_key,              
	SID_Set_ring_Stair,              
	SID_Set_ring_Area,               
	SID_Set_ring_Door,              
	SID_Set_ring_Center,             
	SID_Set_ring_Room,               
	SID_Set_ring_fenji,              
	SID_Set_ring_Info,              
	SID_Set_ring_alarm,              
	SID_Set_ring,                   
	SID_Set_hit,                    
	SID_Set_Lyly_Mode_Audio,         
	SID_Set_Lyly_Mode_Both,         
	SID_Set_Lyly_Mode_Video,         
	SID_Set_Lyly_Link1,              
	SID_Set_Lyly_Link2,             
	SID_Set_Lyly_Link3,              
	SID_Set_Lyly_default_tip,        
	SID_Set_Lyly_record_tip,         
	SID_Set_Screen_InTime,           
	SID_Set_Screen_Lcd,
	SID_Set_Query_Del_All,
	SID_Set_Tips_Fail,
	
	// IP协议转换文字
	SID_Set_binding_status,			//绑定状态
	SID_Set_binding_code,			//绑定码
	SID_Set_ip_ptcAddr,				//ip协议转换器地址
	SID_Set_extension_information,	//分机信息
	SID_Set_binding_code1,
	SID_Set_generate_binding_code,	//生成绑定码
	SID_Set_binding,				//绑定
	SID_Set_get_extension,			//获取分机
	SID_Set_bounded,				//已绑定
	SID_Set_unbounded,				//未绑定
	SID_Set_ExtenNo,				//分机号
	SID_Set_ExtenState,				//状态
	SID_Set_Online,					//在线
	SID_Set_Offline,				//离线
	SID_Set_Bind_OK,
	SID_Set_Bind_Fail,

    // 家电设置文字字符ID
	SID_Jd_Set_Light,                
	SID_Jd_Set_Window,               
	SID_Jd_Set_Kongtiao,             
	SID_Jd_Set_Dianyuan,             
	SID_Jd_Set_Gas,                
	SID_Jd_Set_Adrress,              
	SID_Jd_Set_OpenAddr,             
	SID_Jd_Set_CloseAddr,           
	SID_Jd_Set_Dev_Name1,           
	SID_Jd_Set_Dev_Name2,           
	SID_Jd_Open_All,                
	SID_Jd_Close_All,                
	SID_Jd_Qingjing,                
	SID_Jd_Light,                   
	SID_Jd_KongTiao,                
	SID_Jd_Dianyuan,                 
	SID_Jd_Meiqi,                  
	SID_Jd_Control_Mode,          
	SID_Jd_Signal_Control,           
	SID_Jd_Multi_Control,            
	SID_Jd_Group_No,               
	SID_Jd_Dev_Set_Attr,
	SID_Jd_Dev_Set_Adjust,           
	SID_Jd_Dev_Set_UnAdjust,         
	SID_Jd_Dev_Set_Light,          
	SID_Jd_Dev_Set_Temp,             
	SID_Jd_Dev_Set_NoDev,           
	SID_Jd_Dev_Name,               
	SID_Jd_KongTiao_Learn,           
	SID_Jd_KongTiao_Cold,            
	SID_Jd_KongTiao_Hot,            
	SID_Jd_KongTiao_Mode,           
	SID_Jd_Learn,                    
	SID_Jd_Test,                     
	SID_Jd_KongTiao_Refre,          
	SID_Jd_KongTiao_Warm,          
	SID_Jd_KongTiao_Leisure,         
	SID_Jd_KongTiao_Sleep,          
	SID_Jd_KongTiao_Close,           
	SID_Jd_Button_Cold,             
	SID_Jd_Button_Warm,             
	SID_Jd_Button_Mode,
	SID_Jd_Window_Stop,              
	SID_Jd_Lamp,                    
	SID_Jd_Qingjing1,               
	SID_Jd_Set_Name_Err,          
	SID_Jd_Set_Adr_Err,            
	SID_Jd_Set_OpAdr_Err,           
	SID_Jd_Set_ClAdr_Err,            
	SID_Jd_Set_Dev_Failed,         
	SID_Jd_Set_Dev_Noname,          
	SID_Jd_Adress_Set_Warning,       
	SID_Jd_Set_Scene_Noname,        
	SID_Jd_Set_KtLearn_Err,         
	SID_Jd_Set_KtLearn_Ok,        
	SID_Jd_Set_Adr_Wrong,          
	SID_Jd_Sence_Adr,                
	SID_Jd_Aurine_Adress_Set_Warning,  
	SID_Jd_Kongtiao_Learning,        
	SID_Jd_Set_Scene,            
	SID_Jd_Mode_Huike,               
	SID_Jd_Mode_Jiucan,
	SID_Jd_Mode_Yejian,             
	SID_Jd_Mode_Jiedian,             
	SID_Jd_Mode_Putong,              
	SID_Jd_Mode_Self,               
	SID_Jd_Mode_Set_Name1,          
	SID_Jd_Mode_Set_Name2,        
	SID_Jd_Mode_Out,              
	SID_Jd_Mode_Cook,                
	SID_Jd_Mode_first,              
	SID_Jd_Mode_Afater,            
	SID_Jd_Mode_Night,               
	SID_Jd_Mode_Bed,                
	SID_Jd_Mode_QiYe,               
	SID_Jd_Mode_Up,                 
	SID_Jd_Mode_Muyu,             
	SID_Jd_Mode_Tea,            
	SID_Jd_Mode_Drink,              
	SID_Jd_Mode_Read,              
	SID_Jd_Mode_Work,                
	SID_Jd_Mode_Juhui,
	SID_Jd_Mode_Play,               
	SID_Jd_Mode_Film,                
	SID_Jd_Mode_Sing,                
	SID_Jd_Mode_Storang,             
	SID_Jd_Mode_Wenxi,            
	SID_Jd_Mode_Langman,             
	SID_Jd_Mode_Jiqing,              
	SID_Jd_Mode_Dianya,            
	SID_Jd_Mode_Chlid,               
	SID_Jd_Mode_Chenwen,             
	SID_Jd_Mode_Think,            
	SID_Jd_Mode_Cool,               
	SID_Jd_Mode_Warm,                
	SID_Jd_Mode_Mode,              
	SID_Jd_Mode_Sence,               
	SID_Jd_Mode_Qingdiao,            
	SID_Jd_Mode_Fenwei,            
	SID_Jd_Mode_Time,              
	SID_Jd_Mode_Changjing,      
	SID_Jd_LouShang,
	SID_Jd_LouXia,
	SID_Jd_WuNei,               
	SID_Jd_WuWai,                   
	SID_Jd_TianTai,                
	SID_Jd_GeLou,                  
	SID_Jd_HuaYuan,                  
	SID_Jd_DiJiao,                  
	SID_Jd_TianJin,                  
	SID_Jd_WuYan,                 
	SID_Jd_QianTing,                 
	SID_Jd_HouYuan,           
	SID_Jd_DianTiJian,           
	SID_Jd_TingCheFang,              
	SID_Jd_DaMen,                    
	SID_Jd_QianMen,                
	SID_Jd_ZhongMen,                
	SID_Jd_PangMen,                 
	SID_Jd_HouMen,                  
	SID_Jd_CeMen,                   
	SID_Jd_BianMen,
	SID_Jd_HenMen,                   
	SID_Jd_YanTai,                   
	SID_Jd_Zhoulan,                
	SID_Jd_GuoDao,                 
	SID_Jd_LouTi,                   
	SID_Jd_TiKou,                 
	SID_Jd_TiJian,         
	SID_Jd_TiXia,                   
	SID_Jd_KeTing,                   
	SID_Jd_DaTing,                
	SID_Jd_ZhuTing,               
	SID_Jd_CanTing,              
	SID_Jd_FanTing,   
	SID_Jd_MenTing,                
	SID_Jd_ZhuWoShi,                
	SID_Jd_ZhuRenFang,              
	SID_Jd_WoShi,                 
	SID_Jd_ShuiFang,               
	SID_Jd_FangJianA,             
	SID_Jd_FangJianB, 
	SID_Jd_FangJianC,                
	SID_Jd_FangJianD,        
	SID_Jd_FangJianE,                
	SID_Jd_ChuFang,                  
	SID_Jd_ShuFang,                  
	SID_Jd_KeFang,                   
	SID_Jd_ZhuYuShi,                 
	SID_Jd_YuShi,                  
	SID_Jd_ErTongFang,               
	SID_Jd_GongRenFang,          
	SID_Jd_DuoYongTuJian,            
	SID_Jd_QiJuShi,                  
	SID_Jd_YouLeShi,                 
	SID_Jd_ShuXiShi,               
	SID_Jd_WeiShengJian,            
	SID_Jd_XieMaoJian,              
	SID_Jd_GongZuoJian,              
	SID_Jd_DiXiaShi,                 
	SID_Jd_None,                   
	SID_Jd_Deng,
	SID_Jd_TiaoGuangDeng,           
	SID_Jd_BaiChiDeng,               
	SID_Jd_GuangGuan,               
	SID_Jd_DiaoDeng,                
	SID_Jd_HuaDeng,                  
	SID_Jd_BiDeng,                
	SID_Jd_SheDeng,                 
	SID_Jd_TaiDeng,                 
	SID_Jd_LouDiDeng,            
	SID_Jd_ZhaoMingDeng,            
	SID_Jd_ShiYingDeng,              
	SID_Jd_YeDeng,                   
	SID_Jd_ZhuanshiDEng,       
	SID_Jd_DianZhiSuo,               
	SID_Jd_XiaoduWanGui,           
	SID_Jd_KongQiJinHua,            
	SID_Jd_YinShuiJi,            
	SID_Jd_KongTiaoJi,              
	SID_Jd_LenQiJi,                  
	SID_Jd_CouYouJi,
	SID_Jd_NuanFenJi,               
	SID_Jd_DianReTan,                
	SID_Jd_ReShuiLu,                
	SID_Jd_DianFanBao,               
	SID_Jd_DianZhiWaGuo,             
	SID_Jd_WeiBoLu,                 
	SID_Jd_XiYiJi,               
	SID_Jd_DianShiJi,                
	SID_Jd_LuXiangJi,               
	SID_Jd_YingXiang,              
	SID_Jd_DianNao,                  
	SID_Jd_FengShan,             
	SID_Jd_CD,                  
	SID_Jd_LD,              
	SID_Jd_VCD,                    
	SID_Jd_DVD,                    
	SID_Jd_ChaZuo1,                  
	SID_Jd_ChaZuo2,                 
	SID_Jd_ChaZuo3,                 
	SID_Jd_ChaZuo4,
	SID_Jd_ChaZuo5,               
	SID_Jd_XiaoDuSheBe,             
	SID_Jd_SettingMusic,             
	SID_Jd_ChuangLian,              
	SID_Jd_JuanLian,                 
	SID_Jd_DianDongBuLian,        
	SID_Jd_DianDongShaLian,        
	SID_Jd_SceneNameSet,           
	SID_Jd_SceneActionSet,
	#ifdef _AIR_VOLUME_MODE_
	SID_Jd_Air_Volume,
	SID_Jd_Low_Wind,
	SID_Jd_Medium_Wind,
	SID_Jd_Hight_Wind,
	SID_Jd_Automatic,
	#endif

	// 对讲类字符ID
	SID_MainCall_User,				
	SID_MainCall_FengJi,				
	SID_MainCenter_1,				
	SID_MainCenter_2,				
	SID_MainCenter_3,				
	SID_MainMonitor_Door,			
	SID_MainMonitor_Stair,			
	SID_MainMonitor_Area,			
	SID_MainMonitor_Camera,
	SID_MainMonitor_Home,
	SID_DevManager,                 
	SID_DevArea,                    
	SID_DevStair,                 
	SID_DevRoom,                
	SID_DevPhone,                    
	SID_DevDoorPhone,             
	SID_DevDoorNet,                  
	SID_DevGateWay,                  
	SID_DevIPCamera,                 
	SID_DevFenJi,                    
	SID_DevMaster,                 
	SID_RecordMiss,                  
	SID_RecordIn,                   
	SID_RecordOut,               
	SID_Right_Full,                  
	SID_Inter_HangUP,                
	SID_Inter_Answer,                
	SID_Inter_Unlock,             
	SID_Inter_ReDial,               
	SID_Right_Call,
	SID_Inter_Capture,               
	SID_Inter_Volume,               
	SID_Inter_Next,                 
	SID_Inter_Talk,                  
	SID_Inter_GetList,             
	SID_Inter_Snap,                
	SID_Inter_AudioAdd,          
	SID_Inter_AudioDel,             
	SID_Inter_StopMonitor,           
	SID_Input_OK,                    
	SID_Inter_EnterUserNo,           
	SID_Inter_EnterFJNo,             
	SID_Inter_WrongNo,              
	SID_Inter_NoNotFound,           
	SID_Inter_Connecting,          
	SID_Inter_BeCalling,            
	SID_Inter_Calling,               
	SID_Inter_Talking,              
	SID_Inter_RecordHinting,         
	SID_Inter_Recording,
	SID_Inter_UnlockSuccess,        
	SID_Inter_UnlockFailed,          
	SID_Inter_CaptureSuccess,        
	SID_Inter_CaptureFailed,         
	SID_Inter_ManagerNoSet,          
	SID_Inter_Search,                
	SID_Inter_Monitor,               
	SID_Inter_busy,                 
	SID_Inter_ConnFailed,            
	SID_Inter_SearchFailed,          
	SID_Inter_ElevAuthorize,         
	SID_Inter_ElevAuthOK,           
	SID_Inter_ElevAuthFailed, 
	SID_Auto_Capture,

	// 信息文字字符ID
	SID_Info_Sms,                   
	SID_Info_Centre_Des,             
	 
	// 留言文字字符ID
	SID_Media_Lyly,                  
	SID_Media_Snap,                 
	SID_Media_Num,                  
	SID_Media_Z,                    

	// 便利文字字符ID
	SID_Bl_Jrly,                  
	SID_Bl_ClearScreen,
	
	// 家电文字字符ID
	SID_Jd_Mode_Unused,              
	SID_Jd_Factory_Set,             
	SID_Jd_Factory,                  
	SID_Jd_Factory_X10,            
	SID_Jd_Factory_Suobo,            
	SID_Jd_Factory_ACBUS,           

	// 报警文字字符ID
	SID_Bj_SOS,                    
	SID_Bj_Fire,                     
	SID_Bj_Gas,                     
	SID_Bj_Door,                    
	SID_Bj_Room,                   
	SID_Bj_Big_Window,               
	SID_Bj_Window,                   
	SID_Bj_Balcony,                  
	SID_Bj_Bf_Home,                 
	SID_Bj_Bf_Out,                   
	SID_Bj_Bf_Night,              
	SID_Bj_Clean,                    
	SID_Bj_Baojing_Record,          
	SID_Bj_Caozuo_Rec,               
	SID_Bj_Alarm_Rec_Clear_F,        
	SID_Bj_Fangqu1,					
	SID_Bj_Fangqu2,					
	SID_Bj_Fangqu3,					
	SID_Bj_Fangqu4,					
	SID_Bj_Fangqu5,					
	SID_Bj_Fangqu6,					
	SID_Bj_Fangqu7,					
	SID_Bj_Fangqu8,
	SID_Bj_Fangqu,					
	SID_Bj_Set_Err,                 
	SID_Bj_Fangqu_Settings,          
	SID_Bj_Security_Settings,       
	SID_Bj_Security_Parameter,       
	SID_Bj_Tantou_Type,             
	SID_Bj_24Hour_Fangqu,            
	SID_Bj_Normally_Open,            
	SID_Bj_Normally_Close,          
	SID_Bj_Alarm_Delay,              
	SID_Bj_None,                     
	SID_Bj_Picture,             
	SID_Bj_Voice,                    
	SID_Bj_Enable,                   
	SID_Disable,                     
	SID_Bj_30Seconds,			    
	SID_Bj_1Minute,				    
	SID_Bj_2Minutes,				   
	SID_Bj_3Minutes,				   
	SID_Bj_4Minutes,
	SID_Bj_5Minutes,				   
	SID_Bj_10Minutes,			  
	SID_Bj_15Minutes,			     
	SID_Bj_Alarm_Time,              
	SID_Bj_Early_Warning_Time,       
	SID_Bj_Early_Warning_Voice,      
	SID_Bj_Remote_sz,             
	SID_Bj_Remote_bf,               
	SID_Bj_Remote_cf,               
	SID_Bj_Remote_jf,              
	SID_Bj_Already_Bufang,      
	SID_Bj_Report_Type_Chufa,        
	SID_Bj_Report_Type_Qiuzhu,      
	SID_Bj_Alarm,                   
	SID_Bj_Report_Type_Jinru,       
	SID_Bj_Fangquchufa,            
	SID_Bj_Bengjikongzhi,         
	SID_Bj_Fengjikongzhi,          
	SID_Bj_Query_Del_Rec_One,        
	SID_Bj_Query_Del_Rec_All,
	SID_Bj_Chufa,                    
	SID_Bj_Cannot_Bufang,            
	SID_InputBox_Psw_Err,            
	SID_Isolation,                
	SID_AfJoint_Tk_Chefang,          
	SID_AfJoint_Mk_Chefang,           
	SID_AfJoint_DoorRing,            
	SID_AfJoint_ScenBf,              
	SID_AfJoint_ScenCf,              
	SID_Af_Tel_Num1,                 
	SID_Af_Tel_Num2,                 
	SID_Af_Msg_Num1,                 
	SID_Af_Msg_Num2,                 
	SID_Af_Mode_Unused,           
	SID_Af_Alarm_Cannot_Bf,          
	SID_Af_Touch_Cannot_Bf,        
	SID_Af_Alarm_Cannot_Qc,

	// 预约文字字符ID
	SID_Jd_Yuyue,                 
	SID_Jd_Yuyue_Manager,           
	SID_Jd_Yuyue_All,                
	SID_Jd_Yuyue_DelOne,             
	SID_Jd_Yuyue_Add_Err,            
	SID_Jd_Yuyue_EveryDay,          
	SID_Jd_Yuyue_EveryWeek,          
	SID_Jd_Yuyue_EveryMonth,         
	SID_Jd_Yuyue_Day,                
	SID_Jd_Yuyue_Action_Open,      
	SID_Jd_Yuyue_Action_Close,     
	SID_Jd_Yuyue_Type,            
	SID_Jd_Yuyue_Name,              
	SID_Jd_Yuyue_Status,             
	SID_Jd_Yuyue_Repeat,             
	SID_Jd_Yuyue_StartTime,

	// 社区监视文字字符ID
	SID_Rtsp_IPC,                    
	SID_Rtsp_Name,                   
	SID_Rtsp_Port,                  
	SID_Rtsp_Channel,               
	SID_Rtsp_User,                   
	SID_Rtsp_Password,              
	SID_Rtsp_IP,                     
	SID_Rtsp_Factory,

	// 家居监视文字字符ID
	SID_Rtsp_Indoor_Camera,
	SID_Rtsp_Enable_Indoor_Camera,
	SID_Rtsp_PTZ,
	SID_Rtsp_Direc,
	SID_Rtsp_Scaling,
	SID_Rtsp_Point,
	SID_Rtsp_Point1,
	SID_Rtsp_Point2,
	SID_Rtsp_Point3,
	SID_Rtsp_Point4,

	// 蓝牙门前机文字字符ID
	#ifdef _ADD_BLUETOOTH_DOOR_
	SID_Ble_Door_Unused1,
	SID_Ble_Door_Unused2,
	SID_Ble_Door_First_Record,
	SID_Ble_Door_Second_Record,
	SID_Ble_Door_Operate_Record,	
	SID_Ble_Door_Lock_Record,
	SID_Ble_Door_24H_Record,
	SID_Ble_Door_History_Record, 
	SID_Ble_Door_Snap_Record,	
	SID_Ble_Door_First,
	SID_Ble_Door_Second,
	SID_Set_Unused,   
	SID_Ble_Door_Obtain,
	SID_Ble_Door_Obtain_Err,
	SID_Ble_Door_Lock_Key,
	SID_Ble_Door_Lock_Pwd,
	SID_Ble_Door_Lock_Card,
	SID_Ble_Door_Lock_Finger,
	SID_Ble_Door_Lock_Face,
	SID_Ble_Door_Lock_Guest,
	SID_Ble_Door_Lock_Monitor,
	SID_Ble_Door_Lock_Photo,
	SID_Ble_Door_Lock_Linshi,
	SID_Ble_Door_Lock_Custom,
	SID_Ble_Door_Operate_Del,
	SID_Ble_Door_Operate_Open,
	SID_Ble_Door_Operate_Unopen,
	SID_Ble_Door_Operate_Ring,
	SID_Ble_Door_Dev_Host,
	SID_Ble_Door_Dev_Door,
	SID_Ble_Door_Dev_Photo, 
	SID_Ble_Door_ManInduction,
	SID_Ble_Door_ThreeErr,	
	SID_Ble_Door_TamperAlarm,
	SID_Ble_Door_Visitor,
	SID_Ble_Door1_Snap_Record,
	SID_Ble_Door2_Snap_Record,
	SID_Ble_Door1_Lock24h_Record,
	SID_Ble_Door2_Lock24h_Record,
	SID_Ble_Door1_LockHis_Record,
	SID_Ble_Door2_LockHis_Record,
	SID_Ble_Door1_Operate24h_Record,
	SID_Ble_Door2_Operate24h_Record,
	SID_Ble_Door1_OperateHis_Record,
	SID_Ble_Door2_OperateHis_Record,
	SID_Ble_Door_Room_List,
	SID_Ble_Door_DNS_Address,
	#endif
	
	SID_MAX
}STRID_E;

typedef enum _FontType
{
    Font12X12 = 12, 
    Font16X16 = 16,
    Font20X20 = 20,
    Font24X24 = 24
}FontType;

/*************************************************
  Function:		get_str
  Description: 	根据字符ID获取字符
  Input: 		
	id			图片ID
  Output:		
  	filename	图片的路径
  Return:		无
  Others:
*************************************************/
char * get_str(STRID_E id);

#ifdef __cplusplus
}
#endif
#endif

