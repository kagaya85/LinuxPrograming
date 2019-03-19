use yzmon;	/* 把yzmon设置为缺省数据库 */

/* 查看当前数据库中的表，应该只有6张表 */
show tables;
show function status;
show procedure status;

drop view if exists view_sub_branch1_id;
create view view_sub_branch1_id as
select branch1_id from branch1
where length(branch1_id) = 4;

/* 建立第1小题的view（此处仅为demo，完成作业时删除建立新的即可）*/
drop view if exists view_inner_zj_bid2num;	/* 如果视图已存在则先删除 */
create view view_inner_zj_bid2num as
select branch1_id as bid1, branch1_name as name, count(branch2_id) as bid2num
from branch2 b2 inner join branch1 b1
on b2.branch2_branch1_id = b1.branch1_id
group by branch1_id;

/* 建立第2小题的view */
drop view if exists view_devnum_tmp; 
create view view_devnum_tmp as
select branch2_branch1_id as bid1, count(devorg_id) as devnum
from branch2 inner join devorg
on branch2_id = devorg_branch2_id
group by branch2_branch1_id;

drop view if exists view_inner_zj_devnum;
create view view_inner_zj_devnum as
select bid1, branch1_name as name, sum(devnum) as devnum
from view_devnum_tmp inner join branch1
on bid1 = branch1_id
group by bid1;

/* 建立第3小题的view */
drop view if exists view_devcount_tmp1;
create view view_devcount_tmp1 as
select devorg_branch2_id as branch2_id, count(distinct devstate_base_devid) as devnum
from devorg inner join devstate_base
on devorg_id = devstate_base_devid
group by devorg_id; -- 这里按devorg_id分类有问题

drop view if exists view_devcount_tmp2;
create view view_devcount_tmp2 as
select branch2_branch1_id as bid1, sum(devnum) as devcount
from view_devcount_tmp1 inner join branch2
on view_devcount_tmp1.branch2_id = branch2.branch2_id
group by bid1;

drop view if exists view_inner_zj_devcount;
create view view_inner_zj_devcount as
select bid1, branch1_name as name, devcount
from view_devcount_tmp2 inner join branch1
on bid1 = branch1_id;

/* 建立第4小题的view */
drop view if exists view_devunreg_tmp;
create view view_devunreg_tmp as
-- 选择出所有未登记设备的机构号和序号
select devstate_base_devid as devid, devstate_base_devno as devno
from devstate_base left join devorg
on devstate_base_devid = devorg_id
where devorg_branch2_id is null;

drop view if exists view_inner_zj_devunreg;
create view view_inner_zj_devunreg as
select branch1_id as bid1, branch1_name as name, count(devno) as devunreg
from view_devunreg_tmp inner join branch1
on (left(devid,4) = branch1_id) 
or (left(devid, 2) = branch1_id and not exists (select * from view_sub_branch1_id where left(devid, 4) = branch1_id))
group by branch1_id;

/* 建立第5小题的view */
drop view if exists view_devdup_tmp;
create view view_devdup_tmp as
-- 选择出所有设备的机构号和序号
select devstate_base_devid as devid, devstate_base_devno as devno
from devstate_base
where  devstate_base_devno > '100';

drop view if exists view_inner_zj_devdup;
create view view_inner_zj_devdup as
select branch1_id as bid1, branch1_name as name, count(devno) as devdup
from view_devdup_tmp inner join branch1
on (left(devid,4) = branch1_id) 
or (left(devid, 2) = branch1_id and not exists (select * from view_sub_branch1_id where left(devid, 4) = branch1_id))
group by branch1_id;

/* 建立第6小题的view */
drop view if exists view_devorg_zj;
create view view_devorg_zj as
select a.bid1 as bid1, a.name as name, ifnull(bid2num, 0) as bid2num, ifnull(devnum, 0) as devnum, ifnull(devcount, 0) as devcount, ifnull(devunreg, 0) as devunreg, ifnull(devdup, 0) as devdup
from view_inner_zj_bid2num a left join view_inner_zj_devnum b on a.bid1 = b.bid1 left join view_inner_zj_devcount c on a.bid1 = c.bid1 
left join view_inner_zj_devunreg d on a.bid1 = d.bid1 left join view_inner_zj_devdup e on a.bid1 = e.bid1;

/* 执行第1-6小题视图的验证 */
select * from view_inner_zj_bid2num;
select * from view_inner_zj_devnum;
select * from view_inner_zj_devcount;
select * from view_inner_zj_devunreg;
select * from view_inner_zj_devdup;
select * from view_devorg_zj;

/* 删除你建立的临时表、视图、过程、函数等 */
drop view view_sub_branch1_id;
drop view view_devcount_tmp1;
drop view view_devcount_tmp2;
drop view view_devdup_tmp;
drop view view_devnum_tmp;
drop view view_devorg_zj;
drop view view_devunreg_tmp;
drop view view_inner_zj_bid2num;
drop view view_inner_zj_devcount;
drop view view_inner_zj_devdup;
drop view view_inner_zj_devnum;
drop view view_inner_zj_devunreg;

/* 再次查看当前数据库中的表，应该仅有6张表，如果自己的视图、过程、函数等未删除干净则不得分 */
show tables;
show function status;
show procedure status;
