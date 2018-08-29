<?php

/*
Example of usage:

//path to this script
require_once('git/dev/dev-scripts/php/Output.inc'); 


//path to output-file (if neccessary righs exists,it creates subdirs and file if it doesn't exists)
$output = new Output("/temp/output.txt");


//log any datatype by printing a desc of value
$output->logk('print key == ', $value); 


//log any datatype 
$output->log($value);


// logs seperator lines with date
$output->log_sepDate();


//logs result from command:  ls -altr <directory> 
$output->log_listed_dir('directory'); 


//logs result from command: ls -altr <directory> | grep <file>
$output->log_listed_dir('directory','file.txt');  


//logs entire file content
$output->log_file('file'); 


//logs file content between the 0-10 row, 
$output->log_file('file',0,10); 


// returns the date (does not log)
$output->getDate(); 
*/

class Output {
    private $path_from_root;
    private $full_path;

    function __construct($file) {
        $file = $this->check_if_root_path($file);
        if(strpos($file, '/') !== false){//if contains /
          $arr = explode('/',$file);
          $filename = array_pop($arr);
          $arr = $this->correct_array($arr);
          $dir_arr = $this->dir_arr($arr);
          $this->create_directories($arr,$dir_arr);
          $dir_arr['full_path'] .= $filename;
          $dir_arr['filename'] = $filename;
          $this->full_path = $dir_arr['full_path'];
          $this->create_file($dir_arr['full_path']);
        }else{
          if($this->path_from_root === 1){
            $file="/".$file;
          }
          $this->full_path=$file;
        }
    }

    private function check_if_root_path($file){
      if(substr( $file, 0, 1 ) === '/'){
          $this->path_from_root=1;
          $file = substr( $file, 1, strlen($file));
        }
        return $file;
    }


    private function correct_array($arr){
      $arr = array_filter($arr);
      if(count($arr)<1){
        echo  "\n\n\nIncorrect path\n";
        die;
      }
      return $arr;
    }

    private function dir_arr($arr){
      $dir_path=array();
      $path="";
      if($this->path_from_root === 1){
        $path="/";
      }
      for($i=0;$i<=count($arr);$i++){
        $path .= $arr[$i];
        $dir_path[$arr[$i]] = $path;
        if(($i)<count($arr)){
            $path .= "/";
        }
      }

      $dir_path['full_path'] = $path;
      $dir_path = array_filter($dir_path);
      return $dir_path;
    }

    private function create_directories($arr,$dir_path){
      foreach ($arr as $index) {
        if(!file_exists($dir_path[$index])){
          mkdir($dir_path[$index], 0777); 
        }
      }     
    }

    private function create_file($full_path){
      if(!file_exists($full_path)){
        touch($full_path, 0777);  
      }
    }

  private function write_2_file($output){
    $f =$this->full_path;
    $file = fopen($f, "a") or die("Unable to open file at: " . $f);
    fwrite($file,$output);
    fclose($file);
  }  

  private function read_from_file($path,$from = null,$to = null){
    $handle = fopen($path, "r");
    $line_counter=0;
    if($handle){
      while (($line = fgets($handle)) !== false) {
        $line_counter++;
        if($from && $from>$line_counter){
          continue;    
        }
        if($to && $to < $line_counter){
          break;     
        }
        $out_put .= $line;
      }
      fclose($handle);
    }
    if(is_null($out_put) || trim($out_put)  == ''){
      $out_put = "<-- EMPTY FILE -->\n";
    }
    return $out_put;
  }

  private function complexed_print($msg = '',$var){
      try{
        if(is_resource($var)){
          return $this->v_export($msg, $var);
        }
        else if(is_array($var)){
          return $this->print_arr($msg, $var);
        }else if(is_object($var)){
          return $this->v_export($msg, $var);
        }
        else{
          return $this->print_txt_nr_bool($msg, $var);
        }
      }catch(Exception $e){
        return $this->v_dump($msg, $var);
      }
  }   

  private function v_dump($msg = '', $input){
    $output = var_dump($input,true);
    if($msg == ''){
      return "\n\n".$output."\n";
    }
    return "\n\n".$msg.$output."\n";
  }

  private function v_export($msg = '', $input){
    $output = var_export($input,true);
    $space;
    if($msg == ''){
      return "\n\n".$output."\n";
    }
    return "\n\n".$msg.$output."\n";
  }

  private function print_arr($msg = '', $input){
    $output = print_r($input,true);
    if($msg == ''){
      return "\n\n".$output."\n";
    }
    return "\n\n".$msg.$output."\n"; 
  }

  private function dir_exists($path){
    if(@file_exists($path) && @is_dir($path)){
        return true;
    }
    return false;
  }

  private function validateVariable($input, $extended_msg = null){
    if(is_null($input) ){
      if($extended_msg){
        return "<-- $extended_msg NULL -->";
      }
      return "<-- NULL -->";
    }
    else if(trim($input) == ''){
      if($extended_msg){
        return "<-- $extended_msg EMPTY -->";
      }
      return "<-- EMPTY -->";
    }
    return $input;
  }

  private function is_null_or_empty($input){
    if( is_null($input) || trim($input) == ''){
      return true;
    }
    return false;
  }

  private function print_txt_nr_bool($msg = '', $input){
    $input = $this->validateVariable($input);
    if($msg == ''){
      return "\n\n".$input."\n";
    }
    return "\n\n".$msg.$input."\n";
    
  }

##############################################
############## PUBLIC FUNCTIONS ##############  
##############################################

  public function getDate(){
      date_default_timezone_set("Europe/Stockholm");
      $date = date("Y-m-d H:i:s");
      return $date;
  }

  public function log($output){
      $this->logk('',$output);
  }

  public function logk($msg,$output){# log wth key print:
      $output = $this->complexed_print($msg,$output);
      $this->write_2_file($output);
  }

  public function log_sepDate(){//seperation_date
    $date = $this->getDate();
    $separator = str_repeat("#",99)."\n".
                    str_repeat(" ",10).str_repeat("#",29).
                    " ".$date." ".
                    str_repeat("#",29).str_repeat(" ",10).
                    "\n".str_repeat("#",99);
    $this->log($separator);
  }

  public function log_listed_dir($path, $file = null){
      $tmp_path = $this->validateVariable($path);
      $error=false;
      if($tmp_path != $path){
        $result=$this->validateVariable($path,'path is')."\n";
        $path=$tmp_path;
        $error=true;
      }
      $cmd = "ls -altr ".$path;
      if($file){
              $cmd .= ' | grep "'. $file  .'"';
      }
      $out_put = "command: $cmd\n".
                  "time: ".$this->getDate()."\n".
                  str_repeat("- ",45)."\n";
      if($error==false && !$this->dir_exists($path)){
          $error=true;
          $result="<-- dir does not exist -->\n";
      }
      if($error == false){
        $result=shell_exec($cmd);
      }
      if($error==false && $this->is_null_or_empty($result)){
        $result="<-- dir is empty -->\n";
        if($file){
          $result="<-- file does not exist in dir -->\n";
        }
      }   
      $out_put .= $result;
      $out_put = str_repeat("-",90)."\n". $out_put .str_repeat("-",90);     
      $this->log($out_put);
  }

  public function log_file($path, $from = null ,$to = null){
    $out_put='';
    $path= $this->validateVariable($path);
    $error = false;
    if($path == '<-- NULL -->' ){
        $error = true;
        $out_put .= "<-- path is NULL -->\n";
    }
    if($error == false && $path == '<-- EMPTY -->'){
        $error = true;
        $out_put .= "<-- path is EMPTY -->\n";
    }
    if($error == false && !file_exists($path)){
    	$out_put .= "<-- file does not exist -->\n";
	    $error=true;
    }
    if($from && $to && is_numeric($from) && is_numeric($to) && $from>$to){
        $error = true;
        $out_put .= "<-- [parameter] \$from shouldn't be bigger than \$to -->\n";
    }
    if($error == false){
      $out_put = $this->read_from_file($path,$from,$to);   
    }
    $out_put = "path: $path\n".
	             "time: ".$this->getDate()."\n".
               str_repeat("- ",45)."\n".$out_put;
    $out_put = str_repeat("-",90)."\n". $out_put.str_repeat("-",90);
    $this->log($out_put);
  }
}
?> 
