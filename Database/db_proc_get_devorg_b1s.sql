use yzmon;

/* 查看当前数据库中的表，应该只有6张表 */
show tables;
show function status;
show procedure status;

/* 建立各种临时表、视图、函数、过程等 */
/* 所有计划单列市 */
drop view if exists view_sub_branch1_id;
create view view_sub_branch1_id as
select branch1_id from branch1
where length(branch1_id) = 4;

/* 所有一二级分行id相同项 */
drop view if exists view_direct_branch_id;
create view view_direct_branch_id as
select branch1_id as bid 
from branch1 inner join branch2
on branch1_id = branch2_id;

/* devnum */
/* 按分行序号统计设备数量，建立视图 */
drop view if exists view_devnum; 
create view view_devnum as
select branch2_branch1_id as bid1, branch2_id as bid2, count(devorg_id) as devnum
from branch2 inner join devorg
on branch2_id = devorg_branch2_id
group by branch2_id;

/* devcount */
/* 按分行序号统计所有登记设备数量 */
drop view if exists view_devcount;
create view view_devcount as
select devorg_branch2_id as bid2, count(distinct devstate_base_devid) as devcount
from devorg inner join devstate_base
on devorg_id = devstate_base_devid
group by devorg_branch2_id;

/* devunreg */
-- 选择出所有未登记设备的机构号和序号
drop view if exists view_devunreg;
create view view_devunreg as
select if(
    exists(
        select bid from view_direct_branch_id 
        where bid = left(devstate_base_devid, 2) 
    ),left(devstate_base_devid, 2), left(devstate_base_devid, 4)
) as bid2, count(devstate_base_devid) as devunreg
from devstate_base left join devorg
on devstate_base_devid = devorg_id
where devorg_branch2_id is null
group by bid2;

/* devdup */
drop view if exists view_devdup;
create view view_devdup as
select if(
    exists(
        select bid from view_direct_branch_id 
        where bid = left(devstate_base_devid, 2) 
    ),left(devstate_base_devid, 2), left(devstate_base_devid, 4)
) as bid2, count(devstate_base_devid) as devdup
from devstate_base
where devstate_base_devno > 100
group by bid2;

/* result table */
drop table if exists table_result;
create table table_result as
select branch1_id as bid1, branch1_id as bid2, ifnull(branch2_name, branch1_name) as bid2name, ifnull(devnum, 0), ifnull(devcount, 0), ifnull(devunreg, 0), ifnull(devdup, 0)
from branch1 left join branch2 on branch1_id = branch2_branch1_id 
    left join view_devnum on branch2_id = view_devnum.bid2
    left join view_devcount on branch2_id = view_devcount.bid2
    left join view_devunreg on branch2_id = view_devunreg.bid2
    left join view_devdup on branch2_id = view_devdup.bid2;

/* 如果过程已存在则删除 */
drop procedure if exists proc_get_devorg_b1s;

/* 建立过程 */
create procedure proc_get_devorg_b1s(in bid char(4))
begin
    if bid is null then
        select * from table_reslut;
    else
        select * from table_result
        where bid1 = bid;
    end if
end

/* 生成与answer一样的输出文件的调用顺序，不要改动 */
call proc_get_devorg_b1s(NULL);
call proc_get_devorg_b1s("11");
call proc_get_devorg_b1s("12");
call proc_get_devorg_b1s("13");
call proc_get_devorg_b1s("14");
call proc_get_devorg_b1s("15");
call proc_get_devorg_b1s("21");
call proc_get_devorg_b1s("2102");
call proc_get_devorg_b1s("22");
call proc_get_devorg_b1s("23");
call proc_get_devorg_b1s("31");
call proc_get_devorg_b1s("32");
call proc_get_devorg_b1s("33");
call proc_get_devorg_b1s("3302");
call proc_get_devorg_b1s("34");
call proc_get_devorg_b1s("35");
call proc_get_devorg_b1s("3502");
call proc_get_devorg_b1s("36");
call proc_get_devorg_b1s("37");
call proc_get_devorg_b1s("3702");
call proc_get_devorg_b1s("41");
call proc_get_devorg_b1s("42");
call proc_get_devorg_b1s("43");
call proc_get_devorg_b1s("44");
call proc_get_devorg_b1s("4403");
call proc_get_devorg_b1s("45");
call proc_get_devorg_b1s("46");
call proc_get_devorg_b1s("50");
call proc_get_devorg_b1s("51");
call proc_get_devorg_b1s("52");
call proc_get_devorg_b1s("53");
call proc_get_devorg_b1s("54");
call proc_get_devorg_b1s("61");
call proc_get_devorg_b1s("62");
call proc_get_devorg_b1s("63");
call proc_get_devorg_b1s("64");
call proc_get_devorg_b1s("65");

/* 删除你建立的临时表、视图、过程、函数等 */

drop view if exists view_sub_branch1_id;
drop view if exists view_direct_branch_id;
drop view if exists view_devcount;
drop view if exists view_devdup;
drop view if exists view_devnum;
drop view if exists view_devunreg;
drop table if exists table_result;
drop procedure if exists proc_get_devorg_b1s;

/* 再次查看当前数据库中的表，应该仅有6张表，如果自己的视图、过程、函数等未删除干净则不得分 */
show tables;
show function status;
show procedure status;

